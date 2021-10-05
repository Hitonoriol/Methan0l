#include "ExprEvaluator.h"

#include <iostream>

#include "expression/AssignExpr.h"
#include "expression/ConditionalExpr.h"
#include "expression/IdentifierExpr.h"
#include "expression/BinaryOperatorExpression.h"
#include "expression/PrefixExpr.h"
#include "expression/PostfixExpr.h"
#include "expression/UnitExpr.h"
#include "expression/InvokeExpr.h"
#include "expression/ListExpr.h"
#include "expression/IndexExpr.h"
#include "util.h"

namespace mtl
{

ExprEvaluator::ExprEvaluator()
{
	init_io_oprs();
	init_arithmetic_oprs();
	init_logical_oprs();
	init_comparison_oprs();

	/* Inline concatenation */
	binary_op(TokenType::INLINE_CONCAT, [this](auto lhs, auto rhs) {
		auto lexpr = eval(lhs), rexpr = eval(rhs);
		return Value(lexpr.to_string() + rexpr.to_string());
	});

	/* Return operator */
	postfix_op(TokenType::EXCLAMATION, [this](
			auto lhs) {
				Unit *unit = current_unit();

				if (instanceof<IdentifierExpr>(lhs.get()) && IdentifierExpr::get_name(lhs) == Token::reserved(Word::BREAK))
				unit->stop();
				else
				unit->save_return(eval(lhs));

				return unit->result();
			});

	/* Typeid operator */
	prefix_op(TokenType::TYPE, [this](auto rhs) {
		return Value(static_cast<int>(eval(rhs).type));
	});

	/* Size operator */
	prefix_op(TokenType::SIZE, [this](auto rhs) {
		Value val = eval(rhs);
		int size = 0;

		switch(val.type) {
			case Type::STRING:
			size = val.get<std::string>().size();
			break;

			case Type::LIST:
			size = val.get<ValList>().size();
			break;

			default:
			break;
		}

		return Value(size);
	});

	prefix_op(TokenType::DELETE, [this](auto rhs) {
		scope_lookup(rhs)->del(IdentifierExpr::get_name(rhs));
		return NIL;
	});

	prefix_op(TokenType::EXIT, [this](auto rhs) {
		stop();
		return NO_VALUE;
	});

	prefix_op(TokenType::PERSISTENT, [this](auto rhs) {
		Value rval = eval(rhs);

		if (rval.type != Type::UNIT)
		throw std::runtime_error("Persistence can only be applied to a Unit");

		rval.get<Unit>().set_persisent(true);
		return rval;
	});

	/* Reference an idfr from a unit */
	binary_op(TokenType::DOT, [this](auto lhs,
			auto rhs) {
				Value lval = eval(lhs);

				if (lval.type != Type::UNIT)
					throw std::runtime_error("LHS of dot operator must evaluate to Unit");

				Unit &unit = lval.get<Unit>();
				enter_scope(unit);
				Value result = eval(rhs);
				leave_scope();
				return result;
			});
}

void ExprEvaluator::init_comparison_oprs()
{
	/* Logic-arithmetic operators: >, <, >=, <= */
	TokenType log_ar_ops[] = {
			TokenType::GREATER, TokenType::GREATER_OR_EQ,
			TokenType::LESS, TokenType::LESS_OR_EQ
	};
	for (size_t i = 0; i < std::size(log_ar_ops); ++i)
		binary_op(log_ar_ops[i], [=](auto lhs,
				auto rhs) {
					Value lval = eval(lhs), rval = eval(rhs);
					return Value(eval_logic_arithmetic(lval.as<double>(), log_ar_ops[i], rval.as<double>()));
				});

	binary_op(TokenType::EQUALS, [this](auto lhs, auto rhs) {
		return Value(eval(lhs) == eval(rhs));
	});
}

void ExprEvaluator::init_logical_oprs()
{
	/* Logical operators: BOOL (op) BOOL */
	TokenType log_ops[] = {
			TokenType::AND, TokenType::PIPE
	};
	for (size_t i = 0; i < std::size(log_ops); ++i)
		binary_op(log_ops[i], [=](auto lhs, auto rhs) {
			Value lexpr = eval(lhs), rexpr = eval(rhs);
			return Value(eval_logical(lexpr.as<bool>(), log_ops[i], rexpr.as<bool>()));
		});

	/* Logical NOT operator */
	prefix_op(TokenType::EXCLAMATION, [this](auto rhs) {
		Value rval = eval(rhs);
		return Value(!rval.as<bool>());
	});
}

void ExprEvaluator::init_arithmetic_oprs()
{
	/* Arithmetic operators w\ implicit conversions INTEGER <--> DOUBLE */
	TokenType ar_ops[] = {
			TokenType::PLUS, TokenType::MINUS,
			TokenType::ASTERISK, TokenType::SLASH,
			TokenType::PERCENT, TokenType::POWER
	};
	for (size_t i = 0; i < std::size(ar_ops); ++i)
		binary_op(ar_ops[i], [=](auto lhs, auto rhs) {
			return calculate(lhs, ar_ops[i], rhs);
		});

	prefix_op(TokenType::MINUS, [this](auto rhs) {
		Value rval = eval(rhs);

		if (rval.type == Type::INTEGER)
		rval.value = -rval.as<int>();
		else
		rval.value = -rval.as<double>();

		return rval;
	});

	/* Prefix & Postfix increment / decrement */
	TokenType unary_ar_ops[] = { TokenType::INCREMENT, TokenType::DECREMENT };
	for (size_t i = 0; i < std::size(unary_ar_ops); ++i) {
		prefix_op(unary_ar_ops[i], [=](auto rhs) {
			Value &val = referenced_value(rhs);
			apply_unary(val, unary_ar_ops[i]);
			return val;
		});

		postfix_op(unary_ar_ops[i], [=](auto lhs) {
			Value &val = referenced_value(lhs);
			Value tmp(val);
			apply_unary(val, unary_ar_ops[i]);
			return tmp;
		});
	}
}

void ExprEvaluator::init_io_oprs()
{
	/* Output Operator */
	prefix_op(TokenType::OUT, [this](auto expr) {
		Value rhs = eval(expr);
		std::cout << rhs.to_string();
		return rhs;
	});

	/* Input Operator */
	prefix_op(TokenType::INPUT, [this](auto idfr) {
		IdentifierExpr &idf = try_cast<IdentifierExpr>(idfr);
		std::string input;
		std::cin >> input;
		Value val = Value::from_string(input);
		idf.create_if_nil(*this);
		get(idf) = val;
		return val;
	});
}

Value ExprEvaluator::calculate(ExprPtr &l, TokenType op, ExprPtr &r)
{
	Value lexpr = eval(l), rexpr = eval(r);

	if (Value::is_double_op(lexpr, rexpr))
		return Value(calculate(lexpr.as<double>(), op, rexpr.as<double>()));

	return Value(calculate(lexpr.as<int>(), op, rexpr.as<int>()));
}

ExprEvaluator::ExprEvaluator(Unit &main) : ExprEvaluator()
{
	load_main(main);
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

Value ExprEvaluator::evaluate(BinaryOperatorExpression &opr)
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
		std::cout << '\n' << "Executing " << unit << " Exprs: " << unit.expressions().size() << std::endl;

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

	if constexpr (DEBUG)
		std::cout << "Entered scope " << unit << " // depth: " << exec_stack.size() << std::endl;
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
		std::cout << "Scope lookup for \"" << id << "\"" << std::endl;

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

Value& ExprEvaluator::referenced_value(ExprPtr idfr)
{
	return get(try_cast<IdentifierExpr>(idfr));
}

Value& ExprEvaluator::get(std::string id, bool global)
{
	return scope_lookup(id, global)->get(id);
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
	IdentifierExpr &lhs = try_cast<IdentifierExpr>(expr.get_lhs());
	Value rhs = eval(expr.get_rhs());
	lhs.create_if_nil(*this);
	get(lhs) = rhs;
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

	if (callable.type == Type::UNIT)
		return invoke_unit(expr, callable.get<Unit>());

	else if (callable.type == Type::FUNCTION)
		return invoke(callable.get<Function>(), expr.arg_list().raw_list());

	return NIL;
}

void ExprEvaluator::apply_unary(Value &val, TokenType op)
{
	const int d = unary_diff(op);
	switch (val.type) {
	case Type::INTEGER:
		val.value = val.as<int>() + d;
		break;

	case Type::DOUBLE:
		val.value = val.as<double>() + d;
		break;

	default:
		break;
	}
}

/* Default evaluation */
Value ExprEvaluator::evaluate(Expression &expr)
{
	std::cerr << "Unimplemented expression evaluation" << std::endl;
	return NIL;
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
			std::cout << '\t' << val.first << " = " << val.second << std::endl;
	}
	std::cout << std::endl;
}

} /* namespace mtl */
