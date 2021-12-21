#include "ExprEvaluator.h"

#include <iostream>
#include <memory>

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

#include "lang/core/LibIO.h"
#include "lang/core/LibArithmetic.h"
#include "lang/core/LibLogical.h"
#include "lang/core/LibUnit.h"
#include "lang/core/LibData.h"
#include "lang/core/LibString.h"
#include "lang/core/LibModule.h"

#include "lang/lib/LibMath.h"
#include "lang/structure/File.h"
#include "lang/structure/Random.h"

namespace mtl
{

ExprEvaluator::ExprEvaluator()
{
	load_library(std::make_unique<LibArithmetic>(this));
	load_library(std::make_unique<LibLogical>(this));
	load_library(std::make_unique<LibUnit>(this));
	load_library(std::make_unique<LibIO>(this));
	load_library(std::make_unique<LibString>(this));
	load_library(std::make_unique<LibMath>(this));
	load_library(std::make_unique<LibData>(this));
	load_library(std::make_unique<LibModule>(this));

	type_mgr.register_type(std::make_unique<File>(*this));
	type_mgr.register_type(std::make_unique<Random>(*this));
}

void ExprEvaluator::load_library(std::unique_ptr<Library> library)
{
	library->load();
	libraries.push_back(std::move(library));
}

ExprEvaluator::ExprEvaluator(Unit &main) : ExprEvaluator()
{
	load_main(main);
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

void ExprEvaluator::prefix_op(TokenType tok, PrefixOpr opr)
{
	prefix_ops.emplace(tok, opr);
}

void ExprEvaluator::binary_op(TokenType tok, BinaryOpr opr)
{
	binary_ops.emplace(tok, opr);
}

void ExprEvaluator::postfix_op(TokenType tok, PostfixOpr opr)
{
	postfix_ops.emplace(tok, opr);
}

Value ExprEvaluator::evaluate(BinaryOperatorExpr &opr)
{
	return apply_binary(opr.get_lhs(), opr.get_operator(), opr.get_rhs());
}

Value ExprEvaluator::apply_binary(ExprPtr &lhs, TokenType op, ExprPtr &rhs)
{
	return binary_ops.find(op)->second(lhs, rhs);
}

Value ExprEvaluator::evaluate(PostfixExpr &opr)
{
	return apply_postfix(opr.get_lhs(), opr.get_operator());
}

Value ExprEvaluator::apply_postfix(ExprPtr &lhs, TokenType op)
{
	return postfix_ops.find(op)->second(lhs);
}

Value ExprEvaluator::evaluate(PrefixExpr &opr)
{
	return apply_prefix(opr.get_operator(), opr.get_rhs());
}

Value ExprEvaluator::apply_prefix(TokenType op, ExprPtr rhs)
{
	return prefix_ops.find(op)->second(rhs);
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

Value ExprEvaluator::execute(Unit &unit, const bool use_own_scope)
{
	if constexpr (DEBUG)
		std::cout << '\n' << "Executing " << unit << std::endl;

	if (use_own_scope)
		enter_scope(unit);

	if (exception_handler.is_handling())
		exception_handler.stop_handling();
	else
		unit.reset_execution_state();

	while (unit.has_next_expr()) {
		current_expr = unit.next_expression();
		if (unit.execution_finished())
			break;

		exec(*current_expr);
	}

	Value returned_val = unit.result();
	if (use_own_scope)
		leave_scope();

	if constexpr (DEBUG)
		std::cout << "Finished executing " << unit << '\n' << std::endl;

	if (force_quit())
		return Value::NO_VALUE;

	/* Weak Non-Persistent Units cause their parent units to return */
	Unit *parent = current_unit();
	if (unit.carries_return() && &unit != parent && !returned_val.empty()) {

		if constexpr (DEBUG)
			std::cout << "Carrying return from child weak unit to " << *parent << std::endl;

		parent->save_return(returned_val);
	}

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
	if constexpr (DEBUG)
		dump_stack();

	DataTable::purge_temporary();
	if (exec_stack.size() > 1) {
		Unit *unit = current_unit();

		if (!unit->is_persistent())
			unit->local().clear();
		else
			unit->clear_result();

		pop_tmp_callable();
		exec_stack.pop_back();

		if constexpr (DEBUG)
			std::cout << "<< Left scope // depth: " << exec_stack.size() << std::endl;
	} else {
		if constexpr (DEBUG)
			out << "? Tmp callable stack depth: " << tmp_call_stack.size() << std::endl;
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
	for (auto it = std::prev(exec_stack.end()); it != std::prev(exec_stack.begin()); --it) {
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
	if (global && exec_stack.size() < 2)
		global = false;

	/* If there's an object/box in use, perform lookup in its DataTable first */
	if (!global && !object_stack.empty()) {
		DataTable *objdata = object_stack.back();
		if (objdata->exists(id))
			return objdata;
	}

	const bool weak = current_unit()->is_weak();
	if (!global && !weak)
		return local_scope();

	if constexpr (DEBUG)
		if (weak)
			out << "* Weak Unit lookup" << std::endl;

	/* Starting from the second last Unit, because the last one is the one being executed */
	auto start = std::prev(exec_stack.end(), 2);
	for (auto scope = start; scope != exec_stack.begin(); --scope) {
		/* We shouldn't be able to access Function-local idfrs from outer scopes */
		if (!weak && global && instanceof<Function>(*scope))
			continue;

		DataTable *local = &((*scope)->local());
		if (local->exists(id))
			return local;

		/* Weak Units can locally "see" any identifier above their scope up to the first Regular Unit's scope */
		if (!global && weak && !(*scope)->is_weak())
			return local_scope();
	}

	auto global_scope = this->global();

	/* For weak Units -- if the uppermost scope lookup failed,
	 * 		return the innermost local scope */
	if (!global && !global_scope->exists(id))
		return local_scope();

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

Value& ExprEvaluator::referenced_value(ExprPtr expr, bool follow_refs)
{
	if constexpr (DEBUG)
		out << "? Referencing " << expr->info() << std::endl;

	if (instanceof<IdentifierExpr>(expr))
		return get(try_cast<IdentifierExpr>(expr), follow_refs);
	else if (!instanceof<BinaryOperatorExpr>(expr))
		return DataTable::create_temporary(expr->evaluate(*this));

	auto &dot_expr = try_cast<BinaryOperatorExpr>(expr);
	if (dot_expr.get_operator() != TokenType::DOT)
		throw std::runtime_error("Can't get a reference to a non-dot binary opr");

	ExprPtr lhs = dot_expr.get_lhs(), rhs = dot_expr.get_rhs();
	return instanceof<BinaryOperatorExpr>(lhs) ?
													referenced_value(lhs) :
													dot_operator_reference(lhs, rhs);
}

Value& ExprEvaluator::dot_operator_reference(ExprPtr lhs, ExprPtr rhs)
{
	Value &lref = referenced_value(lhs);
	if (lref.object() && instanceof<IdentifierExpr>(rhs.get())) {
		Object &obj = lref.get<Object>();
		std::string idfr = IdentifierExpr::get_name(rhs);

		if (!obj.has_prv_access() && type_mgr.get_type(obj.type_id()).is_private(idfr))
			throw std::runtime_error(
					"Trying to access a private identifier (" + idfr + ")");

		if constexpr (DEBUG)
			out << "Accessing object field " << idfr << std::endl;

		return obj.field(idfr);
	}
	/*
	if (lref.type() == Type::UNIT)
		return lref.get<Unit>().local().get(Expression::get_name(rhs));
	*/
	return DataTable::create_temporary(apply_binary(lhs, TokenType::DOT, rhs));
}

Value& ExprEvaluator::get(const std::string &id, bool global, bool follow_refs)
{
	Value &val = scope_lookup(id, global)->get(id);
	Value &ret = follow_refs ? val.get() : val;

	if (ret.is<ExprPtr>())
		return DataTable::create_temporary(ret.get<ExprPtr>()->evaluate(*this));

	return ret;
}

Value ExprEvaluator::eval(Expression &expr)
{
	if constexpr (DEBUG)
		expr.info(std::cout << "[Eval] ") << std::endl;

	return expr.evaluate(*this);
}

void ExprEvaluator::exec(Expression &expr)
{
	if constexpr (DEBUG)
		expr.info(std::cout << "[Exec] ") << std::endl;

	return expr.execute(*this);
}

Value ExprEvaluator::evaluate(AssignExpr &expr)
{
	ExprPtr lexpr = expr.get_lhs();
	ExprPtr rexpr = expr.get_rhs();

	/* Move Value from LHS to RHS idfr */
	if (expr.is_move_assignment()) {
		Value lval = eval(lexpr);
		del(lexpr);
		return Value::ref(referenced_value(rexpr) = lval);
	}

	/* Copy assignment */
	Value rhs = eval(rexpr).copy();
	if (instanceof<IdentifierExpr>(lexpr.get())) {
		IdentifierExpr &lhs = try_cast<IdentifierExpr>(lexpr);
		return Value::ref(lhs.assign(*this, rhs));
	}
	else
		return Value::ref(referenced_value(lexpr) = rhs);
}

Value ExprEvaluator::evaluate(ConditionalExpr &expr)
{
	bool result = eval(expr.get_condition()).as<bool>();
	return eval(result ? expr.get_then() : expr.get_else());
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

Value ExprEvaluator::evaluate(InvokeExpr &expr)
{
	Value callable = eval(expr.get_lhs()).get();
	Type ctype = callable.type();

	if constexpr (DEBUG)
		std::cout << "Invoking a callable: " << try_cast<Expression>(&expr).info() << std::endl;

	if (instanceof<IdentifierExpr>(expr.get_lhs())
			&& try_cast<IdentifierExpr>(expr.get_lhs()).get_name() == Token::reserved(Word::SELF_INVOKE)) {
		return invoke(current_function(), expr.arg_list());
	}

	else if (ctype == Type::UNIT) {
		return invoke(callable.get<Unit>(), expr.arg_list());
	}

	else if (ctype == Type::FUNCTION) {
		if (callable.is<InbuiltFunc>())
			return (callable.get<InbuiltFunc>())(expr.arg_list());
		else
			return invoke(callable.get<Function>(), expr.arg_list());
	}

	else if (instanceof<IdentifierExpr>(expr.get_lhs()) && callable.nil())
		return invoke_inbuilt_func(IdentifierExpr::get_name(expr.get_lhs()), expr.arg_list());

	throw std::runtime_error("Invalid invocation expression");
}

bool ExprEvaluator::func_exists(const std::string &name)
{
	return inbuilt_funcs.find(name) != inbuilt_funcs.end();
}

Value ExprEvaluator::invoke_inbuilt_func(std::string name, ExprList args)
{
	auto entry = inbuilt_funcs.find(name);
	if (entry == inbuilt_funcs.end())
		return Value::NO_VALUE;

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

void ExprEvaluator::stop()
{
	current_unit()->stop();
	exec_stack.clear();
	execution_finished = true;
}

bool ExprEvaluator::force_quit()
{
	return execution_finished;
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
	size_t i = 0, depth = exec_stack.size();
	std::cout << "[Execution Stack | Depth: " << depth << "]" << std::endl;

	auto top = std::prev(exec_stack.begin());
	for (auto un = std::prev(exec_stack.end()); un != top; --un) {
		std::cout << ((i++ > 0) ? "  * " : "--> ");
		std::cout << **un;
		if (!(*un)->empty())
			std::cout << " @ line " << (*un)->expressions().front()->get_line();
		if (i == depth)
			std::cout << " (Main)";
		std::cout << std::endl;
		for (auto &val : *(*un)->local().map_ptr()) {
			Value &v = val.second;
			Type type = val.second.type();
			const char quote = (type == Type::STRING || type == Type::CHAR ? '"' : '\0');
			std::cout << '\t'
					<< "(" << v.use_count() << ") "
					<< "["
					<< Value::type_name(type)
					<< " | " << (v.heap_type() ? "heap" : "non-heap")
					<< "] "
					<< val.first << " = "
					<< quote << v << quote << std::endl;
		}
	}
}

} /* namespace mtl */
