#include "ExprEvaluator.h"

#include <iostream>
#include <memory>

#include "Interpreter.h"

#include "util/benchmark.h"

#include "expression/AssignExpr.h"
#include "expression/BinaryOperatorExpr.h"
#include "expression/ConditionalExpr.h"
#include "expression/IdentifierExpr.h"
#include "expression/PrefixExpr.h"
#include "expression/PostfixExpr.h"
#include "expression/UnitExpr.h"
#include "expression/InvokeExpr.h"
#include "expression/ListExpr.h"
#include "expression/IndexExpr.h"
#include "expression/FunctionExpr.h"
#include "util/util.h"

#include "lang/core/library/LibIO.h"
#include "lang/core/library/LibLogical.h"
#include "lang/core/library/LibUnit.h"
#include "lang/core/library/LibData.h"
#include "lang/core/library/LibString.h"
#include "lang/core/library/LibModule.h"
#include "lang/core/library/LibArithmetic.h"
#include "lang/core/library/LibMath.h"
#include "lang/class/File.h"
#include "lang/class/Pair.h"
#include "lang/class/Random.h"

namespace mtl
{

std::unique_ptr<Heap> ExprEvaluator::heap;

STRINGS(
		EnvVars::LAUNCH_ARGS(".argv"),
		EnvVars::SCRDIR(".scrdir"),
		EnvVars::RUNPATH(".rp"),
		EnvVars::RUNDIR(".rd")
)

STRINGS(
		CoreFuncs::LOAD_FILE(".ld")
)

ExprEvaluator::ExprEvaluator()
{
	init_heap();
	load_library<LibArithmetic>();
	load_library<LibLogical>();
	load_library<LibUnit>();
	load_library<LibIO>();
	load_library<LibString>();
	load_library<LibMath>();
	load_library<LibData>();
	load_library<LibModule>();

	type_mgr.register_type<File>();
	type_mgr.register_type<Random>();
	type_mgr.register_type<Pair>();

	register_func("set_max_mem", [](uint64_t cap) {heap->set_max_mem(cap);});
	register_func("mem_in_use", []
	{
		return heap->get_in_use();
	});
	register_func("max_mem", []
	{
		return heap->get_max_mem();
	});
	register_func("enforce_mem_limit", [](bool val) {mtl::HEAP_LIMITED = val;});
	register_func("mem_info", []
	{
		auto in_use = heap->get_in_use(), max = heap->get_max_mem();
		out << "Heap: " << in_use << "/" << max << "b " <<
				"(" << ((static_cast<double>(in_use) / max) * 100) << "% in use)" << NL;
	});
	register_func("on_exit", [&](Value &callable) {
		on_exit_tasks.push_back([=]() mutable {
			Args noargs;
			invoke(callable, noargs);
		});
	});
}

void ExprEvaluator::init_heap(size_t initial_mem_cap)
{
	if (std::pmr::get_default_resource() != std::pmr::new_delete_resource())
		return;

	heap = Heap::create(initial_mem_cap);
	std::pmr::set_default_resource(heap.get());
}

void ExprEvaluator::register_func(const std::string &name, InbuiltFunc &&func)
{
	if (exec_stack.size() <= 1)
		inbuilt_funcs.emplace(name, func);
	else {
		DataTable &table = current_unit()->local();
		table.set(name, func);
	}
}

Value ExprEvaluator::evaluate(BinaryOperatorExpr &opr)
{
	return apply_binary(opr.get_lhs(), opr.get_operator(), opr.get_rhs());
}

Value ExprEvaluator::evaluate(PostfixExpr &opr)
{
	return apply_postfix(opr.get_operator(), opr.get_lhs());
}

Value ExprEvaluator::evaluate(PrefixExpr &opr)
{
	return apply_prefix(opr.get_operator(), opr.get_rhs());
}

void ExprEvaluator::load_main(Unit &main)
{
	if (!exec_stack.empty())
		exec_stack.clear();

	exec_stack.push_back(&main);
}

Unit& ExprEvaluator::get_main()
{
	return *exec_stack.front();
}

const std::string& ExprEvaluator::get_scriptpath()
{
	return global()->get(EnvVars::LAUNCH_ARGS, true).get<ValList>()[0].get<std::string>();
}

const std::string& ExprEvaluator::get_scriptdir()
{
	return global()->get(EnvVars::SCRDIR, true).get<std::string>();
}

Value ExprEvaluator::execute(Unit &unit, const bool use_own_scope)
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

Value ExprEvaluator::invoke(const Function &func, ExprList &args)
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

Value ExprEvaluator::invoke(const Unit &unit, ExprList &args)
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

Value ExprEvaluator::invoke(const Unit &unit)
{
	auto &u = tmp_callable(unit);
	u.call();
	return execute(u);
}

void ExprEvaluator::enter_scope(Unit &unit)
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

void ExprEvaluator::use(Object &obj)
{
	object_stack.push_back(&obj.get_data());
}

void ExprEvaluator::use(Unit &box)
{
	object_stack.push_back(&box.local());
}

void ExprEvaluator::unuse()
{
	object_stack.pop_back();
}

void ExprEvaluator::leave_scope()
{
	IFDBG(dump_stack())

	DataTable::purge_temporary();
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

void ExprEvaluator::restore_execution_state(size_t depth)
{
	if (exec_stack.size() <= depth)
		return;

	exec_stack.erase(exec_stack.begin() + depth, exec_stack.end());
}

size_t ExprEvaluator::stack_depth()
{
	return exec_stack.size();
}

DataTable* ExprEvaluator::global()
{
	return &(exec_stack[0]->local());
}

DataTable* ExprEvaluator::local_scope()
{
	return &(current_unit()->local());
}

Unit* ExprEvaluator::current_unit()
{
	return exec_stack.back();
}

Function& ExprEvaluator::current_function()
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
DataTable* ExprEvaluator::scope_lookup(const std::string &id, bool global)
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

DataTable* ExprEvaluator::scope_lookup(const IdentifierExpr &idfr)
{
	return scope_lookup(idfr.get_name(), idfr.is_global());
}

DataTable* ExprEvaluator::scope_lookup(ExprPtr idfr)
{
	return scope_lookup(try_cast<IdentifierExpr>(idfr));
}

void ExprEvaluator::del(ExprPtr idfr)
{
	if (instanceof<IdentifierExpr>(idfr))
		del(try_cast<IdentifierExpr>(idfr));
	else {
		idfr->assert_type<BinaryOperatorExpr>("Trying to delete an invalid Expression");
		auto dot_expr = try_cast<BinaryOperatorExpr>(idfr);
		if (dot_expr.get_operator() != TokenType::DOT)
			throw std::runtime_error("Can't delete a non-dot operator Expression");
		Value &scope = referenced_value(dot_expr.get_lhs());
		ExprPtr rhs = dot_expr.get_rhs();
		switch (scope.type()) {
		case Type::UNIT:
			scope.get<Unit>().local().del(rhs);
			return;

		case Type::OBJECT:
			scope.get<Object>().get_data().del(rhs);
			return;

		default:
			return;
		}
	}
}

void ExprEvaluator::del(const IdentifierExpr &idfr)
{
	scope_lookup(idfr)->del(idfr.get_name());
}

Value& ExprEvaluator::get(IdentifierExpr &idfr, bool follow_refs)
{
	return idfr.referenced_value(*this, follow_refs);
}

Value& ExprEvaluator::get_env_var(const std::string &name)
{
	return env_table.get(name);
}

void ExprEvaluator::set_env_var(const std::string &name, Value val)
{
	env_table.set(name, val);
}

Value& ExprEvaluator::referenced_value(Expression *expr, bool follow_refs)
{
	LOG("? Referencing " << expr->info())

	/* Reference a named value */
	if (instanceof<IdentifierExpr>(expr))
		return get(try_cast<IdentifierExpr>(expr), follow_refs);

	/* Unwrap reference operaor in-place */
	else if (PrefixExpr::is(*expr, TokenType::REF))
		return referenced_value(try_cast<PrefixExpr>(expr).get_rhs());

	/* Create a temporary value and reference it if `expr` is not an access expression */
	else if (!BinaryOperatorExpr::is(*expr, TokenType::DOT))
		return expr->evaluate(*this).get_ref();

	/* Reference an access expression's value */
	else {
		auto &dot_expr = try_cast<BinaryOperatorExpr>(expr);
		ExprPtr lhs = dot_expr.get_lhs(), rhs = dot_expr.get_rhs();
		return apply_binary(lhs, TokenType::DOT, rhs).get_ref();
	}

	throw std::runtime_error("Reference error");
}

Value& ExprEvaluator::get(const std::string &id, bool global, bool follow_refs)
{
	Value &val = scope_lookup(id, global)->get(id);
	return follow_refs ? val.get() : val;
}

Value ExprEvaluator::eval(Expression &expr)
{
	if constexpr (DEBUG)
		std::cout << "[Eval] " << expr.info() << std::endl;

	return expr.evaluate(*this);
}

void ExprEvaluator::exec(Expression &expr)
{
	if constexpr (DEBUG)
		std::cout << "[Exec] " << expr.info() << std::endl;

	return expr.execute(*this);
}

/* Used when assigning to identifiers.
 * References can only be assigned via explicitly using the `**` operator,
 * even if RHS expression evaluates to a reference without it. */
Value ExprEvaluator::unwrap_or_reference(Expression &expr)
{
	Value val = expr.evaluate(*this);
	if (PrefixExpr::is(expr, TokenType::REF))
		return val;
	return val.get();
}

Value ExprEvaluator::evaluate(AssignExpr &expr)
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
	Value rhs = unwrap_or_reference(*rexpr).copy();
	if (instanceof<IdentifierExpr>(lexpr.get())) {
		IdentifierExpr &lhs = try_cast<IdentifierExpr>(lexpr);
		return Value::ref(lhs.assign(*this, rhs));
	}
	else
		return Value::ref(referenced_value(lexpr) = rhs);
}

Value ExprEvaluator::evaluate(UnitExpr &expr)
{
	return eval(expr);
}

Value ExprEvaluator::evaluate(ListExpr &expr)
{
	Value list = eval(expr);
	return list;
}

Value ExprEvaluator::invoke(const Value &callable, ExprList &args)
{
	return unconst(callable).accept([&](auto &c) {
		HEAP_TYPES(c, {
			IF (std::is_same_v<VT(*c), InbuiltFunc>)
				return (*c)(args);
			ELIF(std::is_same_v<VT(*c), Function>)
				return invoke(*c, args);
			ELIF(std::is_same_v<VT(*c), Unit>)
				return invoke(*c, args);
		})
		return Value::NO_VALUE;
	});
}

Value ExprEvaluator::invoke_method(Object &obj, Value &method, ExprList &args)
{
	auto argcopy = args;
	argcopy.push_front(LiteralExpr::create(obj));
	return method.is<Function>()
			? invoke(method.get<Function>(), argcopy)
						:
				method.get<InbuiltFunc>()(argcopy);
}

Value ExprEvaluator::evaluate(InvokeExpr &expr)
{
	ExprPtr lhs = expr.get_lhs();
	if (instanceof<IdentifierExpr>(lhs)
			&& try_cast<IdentifierExpr>(lhs).get_name()
					== Token::reserved(Word::SELF_INVOKE)) {
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

bool ExprEvaluator::func_exists(const std::string &name)
{
	return inbuilt_funcs.find(name) != inbuilt_funcs.end();
}

Value ExprEvaluator::invoke_inbuilt_func(const std::string &name, ExprList args)
{
	auto entry = inbuilt_funcs.find(name);
	if (entry == inbuilt_funcs.end())
		throw std::runtime_error("Function \"" + name + "\" is not defined");

	if constexpr (DEBUG)
		std::cout << "* Invoking inbuilt function \"" << name << "\"" << std::endl;

	return entry->second(args);
}

/* Default evaluation */
Value ExprEvaluator::evaluate(Expression &expr)
{
	std::cerr << "Unimplemented expression evaluation" << std::endl;
	return Value::NIL;
}

TypeManager& ExprEvaluator::get_type_mgr()
{
	return type_mgr;
}

ExceptionHandler& ExprEvaluator::get_exception_handler()
{
	return exception_handler;
}

void ExprEvaluator::assert_true(bool val, const std::string &msg)
{
	if (!val)
		throw std::runtime_error(msg);
}

void ExprEvaluator::stop()
{
	stopped = true;
	for (auto unit : exec_stack)
		unit->stop();
	on_exit();
}

bool ExprEvaluator::execution_stopped()
{
	return stopped;
}

void ExprEvaluator::on_exit()
{
	LOG("on_exit()")
	for (auto &callable : on_exit_tasks)
		callable();
	on_exit_tasks.clear();
}

InbuiltFuncMap& ExprEvaluator::functions()
{
	return inbuilt_funcs;
}

Expression* ExprEvaluator::get_current_expr()
{
	return current_expr;
}

void ExprEvaluator::dump_stack()
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
			Type type = v.type();
			const char quote = (type == Type::STRING || type == Type::CHAR ? '"' : '\0');
			ss << "(" << v.use_count() << ") "
					<< "[" << Value::type_name(type);
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

} /* namespace mtl */
