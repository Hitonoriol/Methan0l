#include "Interpreter.h"

#include <iostream>
#include <memory>
#include <filesystem>
#include <boost/dll.hpp>

#include <util/util.h>
#include <util/benchmark.h>
#include <util/system.h>

#include <lang/Methan0lParser.h>
#include <expression/AssignExpr.h>
#include <expression/BinaryOperatorExpr.h>
#include <expression/ConditionalExpr.h>
#include <expression/IdentifierExpr.h>
#include <parser/expression/PrefixExpr.h>
#include <parser/expression/PostfixExpr.h>
#include <expression/UnitExpr.h>
#include <expression/InvokeExpr.h>
#include <expression/ListExpr.h>
#include <expression/IndexExpr.h>
#include <expression/FunctionExpr.h>
#include <expression/TryCatchExpr.h>

#include <core/Library.h>
#include <core/Module.h>
#include <core/Internal.h>
#include <oop/Class.h>
#include <CoreLibrary.h>
#include <interpreter/Builtins.h>

namespace mtl
{

STRINGS(
		EnvVars::LAUNCH_ARGS(".argv"),
		EnvVars::SCRDIR(".scrdir"),
		EnvVars::RUNPATH(".rp"),
		EnvVars::RUNDIR(".rd"),
		EnvVars::BIN_PATH(".bp"),
		EnvVars::HOME_DIR(".hd")

)

Interpreter::Interpreter(Protected, InterpreterConfig&& config)
	: heap(Heap::create(config.heap_initial_capacity, config.heap_max_capacity))
	, dlls(allocator<Shared<SharedLibrary>>())
	, libraries(allocator<Shared<Library>>())
	, env_table(this)
	, temporaries(allocator<Value>())
	, inbuilt_funcs(allocator<NativeFuncMap::value_type>())
	, exec_stack(allocator<Unit*>())
	, object_stack(allocator<DataTable*>())
	, tmp_call_stack(allocator<std::shared_ptr<Unit>>())
	, on_exit_tasks(allocator<Task>())
	, main(this)
	, config(std::move(config))
{
	exec_stack.push_back(&main);
}

Interpreter::~Interpreter()
{
	on_exit();
}

void Interpreter::initialize(InitConfig&& initConfig)
{
	this->parser = std::move(initConfig.parser);
	this->evaluator = std::move(initConfig.evaluator);
	load_library<Builtins>();

	if (config.runpath) {
		set_runpath(config.runpath);
		load_libraries();
	}
}

void Interpreter::set_runpath(std::string_view runpath)
{
	/* Path to the working directory from which the interpreter
	 * (or a program that uses the interpreter in embedded setting) has been launched. */
	auto rpath = std::filesystem::absolute(runpath);
	set_env_var(EnvVars::RUNPATH, rpath.string());
	set_env_var(EnvVars::RUNDIR, rpath.parent_path().string());

	/* Path to the binary using the interpreter. */
	auto bin_path = boost::dll::program_location();
	set_env_var(EnvVars::BIN_PATH, bin_path.string());

	/* If interpreter home environment variable is defined, use it as this interpreter's
	 * home directory. Otherwise, use the directory of the binary using this interpreter. */
	auto env_path = mtl::get_env(SystemEnv::MTL_HOME);
	set_env_var(EnvVars::HOME_DIR,
		env_path.empty() ? bin_path.parent_path().string() : env_path
	);
}

void Interpreter::load_libraries()
{
	namespace fs = std::filesystem;
	auto libdir = fs::path(get_home_dir());
	append(libdir, "/" + std::string(LIBRARY_PATH));
	for (auto &entry : fs::directory_iterator(libdir)) {
		auto &path = entry.path();
		auto ext = path.extension();

		// Load a native library
		if (ext == LIBRARY_EXT) {
			try {
				load_shared<ExternalLibrary>(path.string());
			} catch (std::exception &e) {
				std::cerr << "Failed to load library " << path << ": " << e.what() << NL;
			}
		}
		// Load a methan0l module
		else if (ext == PROGRAM_EXT) {
			try {
				Unit module(this);
				core::load_module(*this, path.string(), module);
				execute(module);
				core::import(this, module);
			} catch (...) {
				std::cerr << "Failed to load library " << path << NL;
			}
		}
	}
}

void Interpreter::load_library(std::shared_ptr<Library> lib)
{
	lib->load();
	libraries.push_back(lib);
}

void Interpreter::register_func(const std::string &name, NativeFunc &&func)
{
	LOG("Registered function: " << name)
	if (exec_stack.size() <= 1)
		inbuilt_funcs.emplace(name, func);
	else {
		DataTable &table = current_unit()->local();
		table.set(name, func);
	}
}

Value Interpreter::evaluate(PostfixExpr &opr)
{
	return apply_postfix(opr.get_operator(), opr.get_lhs());
}

Value Interpreter::evaluate(PrefixExpr &opr)
{
	return apply_prefix(opr.get_operator(), opr.get_rhs());
}

std::pair<DataMap::iterator, DataMap::iterator> Interpreter::find_operator_overload(
	Object &obj, TokenType op
)
{
	auto &map = *obj.get_class()->get_class_data().map_ptr();
	return {map.find(Token::to_string(op)), map.end()};
}

void Interpreter::load_main(Unit &main)
{
	if (!exec_stack.empty())
		exec_stack.clear();

	exec_stack.push_back(&main);
}

Unit& Interpreter::get_main()
{
	return *exec_stack.front();
}

const std::string& Interpreter::get_runpath()
{
	return get_env_var(EnvVars::RUNPATH).get<String>();
}

const std::string& Interpreter::get_rundir()
{
	return get_env_var(EnvVars::RUNDIR).get<String>();
}

const std::string& Interpreter::get_bin_path()
{
	return get_env_var(EnvVars::BIN_PATH).get<String>();
}

const std::string& Interpreter::get_home_dir()
{
	return get_env_var(EnvVars::HOME_DIR).get<String>();
}

const std::string& Interpreter::get_scriptpath()
{
	return global()->get(EnvVars::LAUNCH_ARGS, true).get<List>()[0].get<String>();
}

const std::string& Interpreter::get_scriptdir()
{
	return global()->get(EnvVars::SCRDIR, true).get<String>();
}

Value Interpreter::execute(Unit &unit, const bool use_own_scope)
{
	BENCHMARK_START
		LOG('\n' << "Executing " << unit)

	if (use_own_scope)
		enter_scope(unit);

	if (exception_handler.is_handling())
		exception_handler.stop_handling();
	else
		unit.reset_execution_state();

	while (unit.has_next_expr() && !unit.execution_finished())
		exec(*(current_expr = unit.next_expression()));
	clear_temporaries();

	if (execution_stopped()) {
		return Value::NO_VALUE;
	}

	Unit *parent =
			exec_stack.size() > 1 ? *std::prev(exec_stack.end(), 2) : current_unit();
	Value returned_val = unit.result();
	bool carry_return = unit.carries_return();

	/* Handle `break` */
	if (unit.break_performed()) {
		/* Propagate `break` to parent return-carrying Units */
		if (carry_return)
			for (auto it = std::prev(exec_stack.end(), 2);
					it != std::prev(exec_stack.begin()); --it) {
				if ((*it)->carries_return())
					(*it)->stop(true);
			}
	}

	/* `unit` may not exist anymore after the leave_scope() call */
	if (use_own_scope)
		leave_scope();

	LOG("Finished executing " << &unit << '\n')

	/* Handle return carry */
	if (carry_return && !returned_val.empty()) {
		LOG("Carrying return from child weak unit: " << &unit << " to: " << *parent)
		parent->save_return(returned_val);
	}

	BENCHMARK_END
	return returned_val;
}

Value Interpreter::invoke(const Function &func, ExprList &args)
{
	if constexpr (DEBUG) {
		if (!args.empty()) {
			Value f = eval(args.front());
			out << "Function first arg: " << f << std::endl;
		}

		out << "Before function invocation:" << std::endl;
		dump_stack();
	}

	auto &f = tmp_callable(func);
	f.call(*this, args);
	return execute(f);
}

Value Interpreter::invoke(const Unit &unit, ExprList &args)
{
	Unit &u = tmp_callable(unit);
	if (args.empty())
		u.call();

	/* Unit invocation w/ init-block */
	else {
		Value initv = eval(args[0]);
		Unit &init_block = initv.get<Unit>();
		u.call();
		init_block.manage_table(u);
		init_block.set_persisent(true);
		execute(init_block);
		if constexpr (DEBUG)
			std::cout << "Executed Unit Invocation init block " << std::endl;
		init_block.set_persisent(false);
	}

	return execute(u);
}

Value Interpreter::invoke(const Unit &unit)
{
	auto &u = tmp_callable(unit);
	u.call();
	return execute(u);
}

void Interpreter::enter_scope(Unit &unit)
{
	auto local = &unit.local();
	unit.begin();
	if (global() != local)
		exec_stack.push_back(&unit);

	if constexpr (DEBUG) {
		std::cout << ">> Entered scope " << unit << " // depth: " << exec_stack.size() << std::endl;
		dump_stack();
	}
}

void Interpreter::use(Object &obj)
{
	object_stack.push_back(&obj.get_data());
}

void Interpreter::use(Unit &box)
{
	object_stack.push_back(&box.local());
}

void Interpreter::unuse()
{
	object_stack.pop_back();
}

void Interpreter::leave_scope()
{
	IFDBG(dump_stack())

	if (exec_stack.size() > 1) {
		Unit *unit = current_unit();

		if (!unit->is_persistent())
			unit->local().clear();
		else
			unit->clear_result();

		pop_tmp_callable();
		exec_stack.pop_back();
		LOG("<< Left scope // depth: " << exec_stack.size());
	}
	else {
		LOG("Program finished executing, this should be 0: " << tmp_call_stack.size());
		/* If the main unit is persistent, exit handlers can only
		 * be invoked by explicitly calling exit() from the methan0l program. */
		if (!exec_stack.front()->is_persistent())
			on_exit();
	}
}

void Interpreter::restore_execution_state(size_t depth)
{
	if (exec_stack.size() <= depth)
		return;

	exec_stack.erase(exec_stack.begin() + depth, exec_stack.end());
}

size_t Interpreter::stack_depth()
{
	return exec_stack.size();
}

DataTable* Interpreter::global()
{
	return &(exec_stack[0]->local());
}

DataTable* Interpreter::local_scope()
{
	return &(current_unit()->local());
}

Unit* Interpreter::current_unit()
{
	return exec_stack.back();
}

Function& Interpreter::current_function()
{
	for (auto it = std::prev(exec_stack.end()); it != std::prev(exec_stack.begin());
			--it) {
		if (instanceof<Function>(*it))
			return try_cast<Function>(*it);
	}
	throw std::runtime_error("No functions in execution stack");
}

/* Search for identifier in local scope(s)
 * 	or in every possible above the current one if <global> is true */
DataTable* Interpreter::scope_lookup(const std::string &id, bool global)
{
	if constexpr (DEBUG)
		std::cout << (global ? "Global" : "Local")
				<< " scope lookup for \"" << id << "\"" << std::endl;

	/* If we're in the uppermost scope, global lookup is equivalent to the local one */
	if (exec_stack.size() < 2)
		return local_scope();

	/* If there's an object/box in use, perform lookup in its DataTable first */
	if (!global && !object_stack.empty()) {
		DataTable *objdata = object_stack.back();
		if (objdata->exists(id))
			return objdata;
	}

	const bool weak = current_unit()->is_weak();
	DataTable *local = local_scope();
	if (!global && !weak)
		return local;

	if constexpr (DEBUG)
		if (weak)
			out << "* Weak Unit lookup" << std::endl;

	/* Skip current scope when looking up globally */
	auto start = std::prev(exec_stack.end(), global ? 2 : 1);
	for (auto scope = start; scope != exec_stack.begin(); --scope) {
		/* We shouldn't be able to access Function-local idfrs from outer scopes */
		if (!weak && global && instanceof<Function>(*scope))
			continue;

		DataTable *curscope = &((*scope)->local());
		if (curscope->exists(id))
			return curscope;

		/* Weak Units can locally "see" any identifier above their scope up to the first Regular Unit's scope */
		if (!global && weak && !(*scope)->is_weak())
			return local;
	}

	auto global_scope = this->global();

	/* For weak Units -- if the uppermost scope lookup failed,
	 * 		return the innermost local scope */
	if (!global && !global_scope->exists(id))
		return local;

	return global_scope;
}

DataTable* Interpreter::scope_lookup(const IdentifierExpr &idfr)
{
	return scope_lookup(idfr.get_name(), idfr.is_global());
}

DataTable* Interpreter::scope_lookup(ExprPtr idfr)
{
	return scope_lookup(try_cast<IdentifierExpr>(idfr));
}

void Interpreter::del(ExprPtr idfr)
{
	if (instanceof<IdentifierExpr>(idfr))
		del(try_cast<IdentifierExpr>(idfr));
	else {
		idfr->assert_type<BinaryOperatorExpr>("Trying to delete an invalid Expression");
		auto dot_expr = try_cast<BinaryOperatorExpr>(idfr);
		if (dot_expr.get_operator() != Tokens::DOT)
			throw std::runtime_error("Invalid deletion target");
		Value &scope = referenced_value(dot_expr.get_lhs());
		ExprPtr rhs = dot_expr.get_rhs();
		rhs->assert_type<IdentifierExpr>();
		auto type = scope.type();
		if (type == Type::UNIT)
			scope.get<Unit>().local().del(rhs);
		else if (scope.object())
			scope.get<Object>().get_data().del(rhs);
	}
}

void Interpreter::del(const IdentifierExpr &idfr)
{
	scope_lookup(idfr)->del(idfr.get_name());
}

Value& Interpreter::get(IdentifierExpr &idfr, bool follow_refs)
{
	return idfr.referenced_value(*this, follow_refs);
}

Value& Interpreter::get_env_var(const std::string &name)
{
	return env_table.get(name);
}

void Interpreter::set_env_var(const std::string &name, Value val)
{
	env_table.set(name, val);
}

void Interpreter::set_env_var(const std::string &name, const std::string &val)
{
	set_env_var(name, make<String>(val));
}

Value& Interpreter::referenced_value(Expression *expr, bool follow_refs)
{
	LOG("? Referencing " << expr->info())

	/* Reference a named value */
	if (instanceof<IdentifierExpr>(expr))
		return get(try_cast<IdentifierExpr>(expr), follow_refs);

	/* Create a temporary value and reference it if `expr` is not an access expression */
	else if (!BinaryOperatorExpr::is(*expr, Tokens::DOT))
		return evaluate(*expr).get_ref(this);

	/* Reference an access expression's value */
	else {
		auto &dot_expr = try_cast<BinaryOperatorExpr>(expr);
		ExprPtr lhs = dot_expr.get_lhs(), rhs = dot_expr.get_rhs();
		return apply_binary(lhs, Tokens::DOT, rhs).get_ref(this);
	}

	throw std::runtime_error("Reference error");
}

Value& Interpreter::get(const std::string &id, bool global, bool follow_refs)
{
	Value &val = scope_lookup(id, global)->get(id);
	return follow_refs ? val.get() : val;
}

Value Interpreter::eval(Expression &expr)
{
	LOG("[Eval] " << expr.info());
	return evaluate(expr);
}

void Interpreter::exec(Expression &expr)
{
	LOG("[Exec] " << expr.info());

	execute(expr);
	clear_temporaries();
}

void Interpreter::clear_temporaries(size_t up_to)
{
	if (up_to == 0) {
		temporaries.clear();
		return;
	}

	if (up_to == temporaries.size())
		return;

	temporaries.erase(temporaries.begin() + up_to, temporaries.end());
}

size_t Interpreter::current_temporary_depth()
{
	return temporaries.size();
}

/* Used when assigning to identifiers. */
Value Interpreter::unwrap_or_reference(Expression &expr)
{
	return evaluate(expr).get();
}

Value Interpreter::evaluate(AssignExpr &expr)
{
	ExprPtr lexpr = expr.get_lhs();
	ExprPtr rexpr = expr.get_rhs();

	/* Move (assign w/o copying) value of RHS to LHS
	 * + if RHS is an identifier, delete it */
	if (expr.is_move_assignment()) {
		Value rval = unwrap_or_reference(*rexpr);
		if (instanceof<IdentifierExpr>(rexpr))
			del(rexpr);
		return Value::ref(referenced_value(lexpr) = rval);
	}

	/* Copy assignment */
	Value rhs = unwrap_or_reference(*rexpr).copy(this);
	if (instanceof<IdentifierExpr>(lexpr.get())) {
		IdentifierExpr &lhs = try_cast<IdentifierExpr>(lexpr);
		return Value::ref(lhs.assign(*this, rhs));
	}
	else
		return Value::ref(referenced_value(lexpr) = rhs);
}

Value Interpreter::evaluate(UnitExpr &expr)
{
	return eval(expr);
}

Value Interpreter::evaluate(ListExpr &expr)
{
	Value list = eval(expr);
	return list;
}

Value Interpreter::invoke(const Value &callable, ExprList &args)
{
	return unconst(callable).accept([&](auto &c) {
		HEAP_TYPES(c, {
			IF (std::is_same_v<VT(*c), NativeFunc>)
				return (*c)(args);
			ELIF(std::is_same_v<VT(*c), Function>)
				return invoke(*c, args);
			ELIF(std::is_same_v<VT(*c), Unit>)
				return invoke(*c, args);
		})
		return Value::NO_VALUE;
	});
}

Value Interpreter::invoke_method(Object &obj, Value &method, Args &args)
{
	ExprList argcopy = args;
	argcopy.push_front(LiteralExpr::create(obj));
	return method.is<Function>()
			? invoke(method.get<Function>(), argcopy)
			: method.get<NativeFunc>()(argcopy);
}

Value Interpreter::evaluate(InvokeExpr &expr)
{
	ExprPtr lhs = expr.get_lhs();
	if (instanceof<IdentifierExpr>(lhs)
			&& try_cast<IdentifierExpr>(lhs).get_name()
					== ReservedWord::SELF_INVOKE) {
		return invoke(current_function(), expr.arg_list());
	}

	Value callable = eval(lhs).get();
	if (callable.nil())
		if_instanceof<IdentifierExpr>(*lhs, [&](IdentifierExpr &fidfr) {
			auto &fname = fidfr.get_name();
			callable = scope_lookup(fname, true)->get(fname).get();
		});

	if constexpr (DEBUG)
		std::cout << "Invoking a callable: " << try_cast<Expression>(&expr).info() << std::endl;

	if (!callable.nil())
		return invoke(callable, expr.arg_list());

	else if (instanceof<IdentifierExpr>(lhs))
		return invoke_inbuilt_func(IdentifierExpr::get_name(expr.get_lhs()), expr.arg_list());

	throw std::runtime_error("Invalid invocation expression");
}

bool Interpreter::func_exists(const std::string &name)
{
	return inbuilt_funcs.find(name) != inbuilt_funcs.end();
}

Value Interpreter::invoke_inbuilt_func(const std::string &name, const ExprList &args)
{
	auto entry = inbuilt_funcs.find(name);
	if (entry == inbuilt_funcs.end())
		throw std::runtime_error("Function \"" + name + "\" is not defined");

	if constexpr (DEBUG)
		std::cout << "* Invoking inbuilt function \"" << name << "\"" << std::endl;

	return entry->second(args);
}

Value Interpreter::evaluate(Expression& expr)
{
	return evaluator->evaluate(expr);
}

void Interpreter::execute(Expression& expr)
{
	evaluator->execute(expr);
}

TypeManager& Interpreter::get_type_mgr()
{
	return type_mgr;
}

ExceptionHandler& Interpreter::get_exception_handler()
{
	return exception_handler;
}

Heap& Interpreter::get_heap()
{
	if (heap == nullptr)
		throw std::runtime_error("Interpreter heap isn't initialized yet.");

	return *heap;
}

void Interpreter::assert_true(bool val, const std::string &msg)
{
	if (!val)
		throw std::runtime_error(msg);
}

void Interpreter::stop()
{
	stopped = true;
	for (auto unit : exec_stack)
		unit->stop();
	on_exit();
}

bool Interpreter::execution_stopped()
{
	return stopped;
}

void Interpreter::on_exit()
{
	LOG("on_exit()")
	for (auto &callable : on_exit_tasks)
		callable();
	on_exit_tasks.clear();
}

void Interpreter::register_exit_task(Value &callable)
{
	on_exit_tasks.push_back([=, this]() mutable {
		Args noargs;
		invoke(callable, noargs);
	});
}

NativeFuncMap& Interpreter::functions()
{
	return inbuilt_funcs;
}

Expression* Interpreter::get_current_expr()
{
	return current_expr;
}

void Interpreter::dump_stack()
{
	sstream ss;
	size_t i = 0, depth = exec_stack.size();
	ss << "[Execution Stack | Depth: " << depth << "]" << NLTAB;

	auto top = std::prev(exec_stack.begin());
	for (auto un = std::prev(exec_stack.end()); un != top; --un) {
		ss << ((i++ > 0) ? "  * " : "--> ");
		ss << **un;
		if (!(*un)->empty())
			ss << " @ line " << (*un)->expressions().front()->get_line();
		if (i == depth)
			ss << " (Main)";

		ss << NL << "Local variables:" << NLTAB;
		auto &table = *(*un)->local().map_ptr();
		auto last = table.end();
		for (auto val = table.begin(); val != table.end(); ++val) {
			Value &v = val->second;
			auto type = v.type();
			const char quote = (type.is<String>() || type == Type::CHAR ? '"' : '\0');
			ss << "(" << v.use_count() << ") "
					<< "[" << type.type_name();
			if (type == Type::REFERENCE)
				ss << " -> " << v.get().identity();
			ss << " | " << (v.heap_type() ? "heap" : "non-heap");
			ss << " " << v.identity();

			if (std::next(val) == last)
				ss << UNTAB << UNTAB;

			ss << "] " << val->first << " = " << quote << v << quote << NL;
		}
		ss << UNTAB;
	}
	out << mtl::tab(ss);
}

void Interpreter::lex(std::string &code)
{
	parser->get_lexer().lex(code, true);
}

bool Interpreter::load()
{
	return try_load([&]() {
		parser->parse_all();
		Unit unit(this, parser->result());
		load(unit);
	});
}

void Interpreter::preserve_data(bool val)
{
	main.set_persisent(val);
}

bool Interpreter::load_program(const std::string &path, bool change_cwd)
{
	bool loaded = load(load_file(path));
	if (loaded) {
		auto program_path = core::path(*this, path);
		auto program_path_abs = core::absolute_path(*this, path);
		set_program_globals(std::filesystem::path(program_path_abs).string());
		if (change_cwd) {
			std::filesystem::current_path(
				std::filesystem::path(program_path_abs).parent_path()
			);
		}
	}
	return loaded;
}

Unit Interpreter::load_file(const std::string &path)
{
	std::ifstream src_file(core::path(*this, path), std::ios::binary | std::ios::ate);

	if (!src_file.is_open()) {
		std::cerr << "Failed to open file: \"" + path + "\"" << std::endl;
		return Unit(this);
	}

	Unit unit = load_unit(src_file);
	src_file.close();
	return unit;
}

Unit Interpreter::load_unit(std::istream &codestr)
{
	std::string code = mtl::read_file(codestr);
	return load_unit(code);
}

Unit Interpreter::load_unit(std::string &code)
{
	if (!try_load([&]() {parser->load(code);}))
		return Unit(this);

	Unit unit(this, parser->result());
	parser->clear();
	parser->reset();
	parser->get_lexer().reset();
	return unit;
}

bool Interpreter::load(std::istream &codestr)
{
	return load(load_unit(codestr));
}

bool Interpreter::load(std::string &code)
{
	return load(load_unit(code));
}

bool Interpreter::load(const Unit &main)
{
	load_main(this->main = main);
	return !main.empty();
}

Unit& Interpreter::program()
{
	return main;
}

Parser& Interpreter::get_parser()
{
	return *parser;
}

Value Interpreter::run()
{
	LOG("Running parsed program...")

	if (main.empty()) {
		IFDBG(std::cerr << "Main Unit is empty. Nothing to run." << std::endl)
		return Value::NO_VALUE;
	}

	Value ret;
	auto &handler = get_exception_handler();
	do {
		try {
			ret = execute(*current_unit());
		} catch (const std::exception &e) {
			if (!handle_exception(e))
				return -1;
		} catch (Value &e) {
			if (!handle_exception(e))
				return -1;
		} catch (...) {
			std::cerr << "[Unknown runtime error]" << std::endl;
		}
	} while (handler.is_handling());

	LOG("Program execution finished")
	return ret;
}

void Interpreter::load_args(int argc, char **argv, int start_from)
{
	std::vector<std::string> list;
	for (int i = start_from; i < argc; ++i)
		list.emplace_back(argv[i]);

	load_args(list);
}

void Interpreter::load_args(const std::vector<std::string> &args)
{
	auto list = make_as<List>([&](auto &list) {
		// This path is set after loading a program
		std::string launch_path_str(main.local().get(EnvVars::SCRDIR, true).get<String>());
		std::string scr_name(std::filesystem::path(core::path(*this, args[0])).filename().string());
		auto launch_path = make<String>(std::filesystem::path(launch_path_str + "/" + scr_name).make_preferred().string());
		list.push_back(launch_path);
		for (auto it = std::next(args.begin()); it != args.end(); ++it) {
			list.push_back(make<String>(*it));
		}
	});
	main.local().set(EnvVars::LAUNCH_ARGS, list);
}

void Interpreter::set_program_globals(const std::string &scrpath)
{
	auto &table = main.local();
	table.set(EnvVars::SCRDIR, make<String>(std::filesystem::absolute(scrpath).parent_path().string()));
}

void Interpreter::syntax_error(const std::exception &e)
{
	std::cerr << "[Syntax error] "
			<< e.what()
			<< " @ line " << parser->get_lexer().next(true).get_line()
			<< std::endl;
	parser->dump_queue();
}

void Interpreter::handle_exception(ExHandlerEntry exentry)
{
	pop_tmp_callable();
	restore_execution_state(exentry.first);
	exentry.second->except(*this);
	exception_handler.start_handling();
}

void Interpreter::unhandled_exception(const std::string &msg)
{
	std::cerr << "[Runtime error] "
			<< msg
			<< " @ line " << get_current_expr()->get_line()
			<< " in expression: " << NL << tab(indent(get_current_expr()->info()))
			<< std::endl;
}

void Interpreter::print_info()
{
	out << "Core language structures:\n"
			<< "* Value: " << sizeof(Value) << NL
			<< "* Reference: " << sizeof(ValueRef) << NL
			<< "* Object: " << sizeof(Object) << NL
			<< "* String: " << sizeof(std::string) << NL
			<< "* Unit: " << sizeof(Unit) << NL
			<< "* Function: " << sizeof(Function) << NL
			<< "* NoValue: " << sizeof(NoValue) << NL
			<< "* Nil: " << sizeof(Nil) << NL
			<< NL;

	out << "Containers:\n"
			<< "* DataTable: " << sizeof(DataTable) << NL
			<< "* List: " << sizeof(ValList) << NL
			<< "* Map: " << sizeof(ValMap) << NL
			<< NL;

	out << "Internal structures:\n"
			<< "* Interpreter: " << sizeof(Interpreter) << NL
			<< "* Expression: " << sizeof(Expression) << NL
			<< "* Token: " << sizeof(Token) << NL
			<< "* Library: " << sizeof(Library) << NL
			<< "* Class: " << sizeof(Class) << NL
			<< "* NativeFunc: " << sizeof(NativeFunc) << NL
			<< "* Allocator: " << sizeof(Allocator<Value>) << NL
			<< NL;
}

} /* namespace mtl */
