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
#include "util/util.h"

#include "lang/core/LibIO.h"
#include "lang/core/LibArithmetic.h"
#include "lang/core/LibLogical.h"
#include "lang/core/LibUnit.h"
#include "lang/core/LibData.h"
#include "lang/core/LibString.h"

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

void ExprEvaluator::inbuilt_func(std::string func_name, InbuiltFunc func)
{
	inbuilt_funcs.emplace(func_name, func);
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

Value ExprEvaluator::execute(Unit &unit)
{
	if constexpr (DEBUG)
		std::cout << '\n' << "Executing " << unit << std::endl;

	enter_scope(unit);

	for (auto expr : unit.expressions()) {
		if (unit.execution_finished())
			break;

		exec(expr);
	}

	Value returned_val = unit.result();
	leave_scope();

	if constexpr (DEBUG)
		std::cout << "Finished executing " << unit << '\n' << std::endl;

	if (force_quit())
		return NO_VALUE;

	/* Weak Units cause their parent units to return */
	Unit *parent = current_unit();
	if (&unit != parent && !returned_val.empty() && unit.is_weak()) {

		if constexpr (DEBUG)
			std::cout << "Carrying return from child weak unit to " << *parent << std::endl;

		parent->save_return(returned_val);
	}

	return returned_val;
}

Value ExprEvaluator::invoke(Function &func, ExprList &args)
{
	func.call(*this, args);
	return execute(func);
}

void ExprEvaluator::enter_scope(Unit &unit)
{
	auto local = &unit.local();
	unit.begin();
	if (global() != local)
		exec_stack.push_back(&unit);

	if constexpr (DEBUG) {
		std::cout << "Entered scope " << unit << " // depth: " << exec_stack.size() << std::endl;
		dump_stack();
	}
}

void ExprEvaluator::leave_scope()
{
	if constexpr (DEBUG)
		dump_stack();

	if (exec_stack.size() > 1) {
		Unit *unit = current_unit();

		if (!unit->is_persistent())
			unit->local().clear();
		else
			unit->clear_result();

		exec_stack.pop_back();

		if constexpr (DEBUG)
			std::cout << "Left scope // depth: " << exec_stack.size() << std::endl;
	}
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

/* Search for identifier in local scope(s)
 * 	or in every possible above the current one if <global> is true */
DataTable* ExprEvaluator::scope_lookup(const std::string &id, bool global)
{
	if constexpr (DEBUG)
		std::cout << " Scope lookup for \"" << id << "\"" << std::endl;

	if (global && exec_stack.size() < 2)
		global = false;

	const bool weak = current_unit()->is_weak();
	if (!global && !weak)
		return local_scope();

	/* Starting from the second last Unit, because the last is the one being executed */
	auto start = std::prev(exec_stack.end(), 2);
	for (auto scope = start; scope != exec_stack.begin(); --scope) {
		DataTable *local = &((*scope)->local());
		if (local->exists(id))
			return local;

		/* Weak Units can locally "see" any identifier above their scope up to the first Regular Unit's scope */
		if (!global && weak && !(*scope)->is_weak())
			return local_scope();
	}

	auto global_scope = this->global();

	/* For weak Units */
	if (!global && !global_scope->exists(id))
		return local_scope();

	return global_scope;
}

DataTable* ExprEvaluator::scope_lookup(IdentifierExpr &idfr)
{
	return scope_lookup(idfr.get_name(), idfr.is_global());
}

DataTable* ExprEvaluator::scope_lookup(ExprPtr idfr)
{
	return scope_lookup(try_cast<IdentifierExpr>(idfr));
}

Value& ExprEvaluator::get(IdentifierExpr &idfr)
{
	return idfr.referenced_value(*this);
}

Value& ExprEvaluator::referenced_value(ExprPtr expr)
{
	if (instanceof<IdentifierExpr>(expr.get()))
		return get(try_cast<IdentifierExpr>(expr));

	auto &dot_expr = try_cast<BinaryOperatorExpr>(expr);
	if (dot_expr.get_operator() != TokenType::DOT)
		throw std::runtime_error("Can't get a reference to a non-dot binary opr");

	ExprPtr lhs = dot_expr.get_lhs(), rhs = dot_expr.get_rhs();
	return instanceof<BinaryOperatorExpr>(lhs.get()) ?
														referenced_value(lhs) :
														dot_operator_reference(lhs, rhs);
}

Value& ExprEvaluator::dot_operator_reference(ExprPtr lhs, ExprPtr rhs)
{
	Value &lref = get(try_cast<IdentifierExpr>(lhs));
	if (lref.object() && instanceof<IdentifierExpr>(rhs.get())) {
		Object &obj = lref.get<Object>();
		std::string idfr = IdentifierExpr::get_name(rhs);

		if (!obj.has_prv_access() && type_mgr.get_type(obj.type_id()).is_private(idfr))
			throw std::runtime_error(
					"Trying to access a private identifier (" + idfr + ")");

		return obj.field(idfr);
	}

	if (lref.type == Type::UNIT)
		return lref.get<Unit>().local().get(IdentifierExpr::get_name(rhs));

	throw std::runtime_error("Invalid use of dot operator");
}

Value& ExprEvaluator::get(const std::string &id, bool global)
{
	return scope_lookup(id, global)->get(id).get();
}

inline Value ExprEvaluator::eval(Expression &expr)
{
	if constexpr (DEBUG)
		expr.info(std::cout << "[Eval] ") << std::endl;

	return expr.evaluate(*this);
}

Value ExprEvaluator::eval(ExprPtr expr)
{
	return eval(*expr);
}

inline void ExprEvaluator::exec(ExprPtr expr)
{
	if constexpr (DEBUG)
		expr->info(std::cout << "[Exec] ") << std::endl;

	return expr->execute(*this);
}

Value ExprEvaluator::evaluate(AssignExpr &expr)
{
	ExprPtr lexpr = expr.get_lhs();
	Value rhs = eval(expr.get_rhs());
	if (instanceof<IdentifierExpr>(lexpr.get())) {
		IdentifierExpr &lhs = try_cast<IdentifierExpr>(lexpr);
		lhs.assign(*this, rhs);
	}
	else
		referenced_value(lexpr) = rhs;
	return rhs;
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

Value ExprEvaluator::invoke_unit(InvokeExpr &expr, Unit &unit)
{
	auto args_expr = expr.arg_list();
	if (args_expr.raw_list().empty())
		unit.call();

	/* Pseudo-args syntax: pass a single Unit with local scope init as argument;
	 * arg Unit will be executed and its data table will be shared with the one being invoked */
	else {
		Value &args = eval(args_expr).get<ValList>()[0];
		Unit ps_args = args.as<Unit>();
		ps_args.call();
		unit.manage_table(ps_args);
		ps_args.set_persisent(true);
		execute(ps_args);
		if constexpr (DEBUG)
			std::cout << "Executed Unit Invocation init block " << std::endl;
		ps_args.set_persisent(false);
	}

	return execute(unit);
}

Value ExprEvaluator::evaluate(InvokeExpr &expr)
{
	Value callable = eval(expr.get_lhs());

	if constexpr (DEBUG)
		std::cout << "Invoking a callable..." << std::endl;

	if (callable.type == Type::UNIT)
		return invoke_unit(expr, callable.get<Unit>());

	else if (callable.type == Type::FUNCTION)
		return invoke(callable.get<Function>(), expr.arg_list().raw_list());

	else if (instanceof<IdentifierExpr>(expr.get_lhs().get()) && callable.empty())
		return invoke_inbuilt_func(IdentifierExpr::get_name(expr.get_lhs()), expr.arg_list().raw_list());

	throw std::runtime_error("Attempting to call a non-existent function");
}

Value ExprEvaluator::invoke_inbuilt_func(std::string name, ExprList args)
{
	auto entry = inbuilt_funcs.find(name);
	if (entry == inbuilt_funcs.end())
		return NO_VALUE;

	if constexpr (DEBUG)
		std::cout << "* Invoking inbuilt function \"" << name << "\"" << std::endl;

	return entry->second(args);
}

/* Default evaluation */
Value ExprEvaluator::evaluate(Expression &expr)
{
	std::cerr << "Unimplemented expression evaluation" << std::endl;
	return NIL;
}

TypeManager& ExprEvaluator::get_type_mgr()
{
	return type_mgr;
}

void ExprEvaluator::stop()
{
	current_unit()->stop();
	exec_stack.clear();
}

bool ExprEvaluator::force_quit()
{
	return exec_stack.empty();
}

InbuiltFuncMap& ExprEvaluator::functions()
{
	return inbuilt_funcs;
}

void ExprEvaluator::dump_stack()
{
	size_t i = 0, depth = exec_stack.size();
	std::cout << "[Execution Stack | Depth: " << depth << "]" << std::endl;

	for (auto un = std::prev(exec_stack.end()); un != std::prev(exec_stack.begin());
			--un) {
		std::cout << ((i++ > 0) ? "  * " : "--> ") << **un;
		if (i == depth)
			std::cout << " (Main)";
		std::cout << std::endl;
		for (auto val : (*un)->local().managed_map())
			std::cout << '\t'
					<< "[" << Value::type_name(val.second.type) << "] "
					<< val.first << " = " << val.second << std::endl;
	}
}

} /* namespace mtl */
