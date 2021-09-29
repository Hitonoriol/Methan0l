#include "ExprEvaluator.h"

#include "expression/AssignExpr.h"
#include "expression/ConditionalExpr.h"
#include "expression/IdentifierExpr.h"
#include "expression/BinaryOperatorExpression.h"
#include "expression/PrefixExpr.h"
#include "expression/PostfixExpr.h"
#include "expression/UnitExpr.h"
#include "expression/InvokeExpr.h"
#include "expression/ListExpr.h"

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
	postfix_op(TokenType::EXCLAMATION, [this](auto lhs) {
		auto unit = current_unit();
		unit->save_return(eval(lhs));
		return local_scope()->get(Unit::RETURN_KEYWORD);
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
			TokenType::PERCENT
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
			Value &val = get(IdentifierExpr::get_name(rhs));
			apply_unary(val, unary_ar_ops[i]);
			return val;
		});

		postfix_op(unary_ar_ops[i], [=](auto rhs) {
			Value &val = get(IdentifierExpr::get_name(rhs));
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
		std::string name = IdentifierExpr::get_name(idfr);
		std::string input;
		std::cin >> input;
		Value val = Value::from_string(input);
		set(name, val);
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
	set_main(main);
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

Value ExprEvaluator::apply_prefix(TokenType op, ExprPtr &rhs)
{
	return prefix_ops.find(op)->second(rhs);
}

void ExprEvaluator::set_main(Unit &main)
{
	if (!exec_queue.empty())
		exec_queue.clear();

	exec_queue.push_back(&main);
}

Value ExprEvaluator::execute(Unit &unit)
{
	if constexpr (DEBUG)
		std::cout << unit << " Exprs: " << unit.expressions().size() << std::endl;

	enter_scope(unit);

	if (!unit.execution_finished())
		for (auto expr : unit.expressions()) {
			exec(expr);
			if (unit.execution_finished())
				break;
		}

	Value returned_val = unit.result();
	leave_scope();

	/* Weak Units cause their parent units to return */
	Unit *parent = current_unit();
	if (&unit != parent && !returned_val.empty() && unit.is_weak())
		parent->save_return(returned_val);

	return returned_val;
}

Value ExprEvaluator::invoke(Function &func, ValList args)
{
	func.call(args);
	return execute(func);
}

void ExprEvaluator::enter_scope(Unit &unit)
{
	if constexpr (DEBUG)
		std::cout << "Entering scope " << unit << std::endl;

	auto local = &unit.local();
	unit.begin();
	if (global() != local)
		exec_queue.push_back(&unit);
}

void ExprEvaluator::leave_scope()
{
	if (exec_queue.size() > 1) {
		exec_queue.back()->local().clear();
		exec_queue.pop_back();

		if constexpr (DEBUG)
			std::cout << "Left local scope // Cur visibility: " << exec_queue.size() << std::endl;
	}
}

DataTable* ExprEvaluator::global()
{
	return &(exec_queue[0]->local());
}

DataTable* ExprEvaluator::local_scope()
{
	return &(exec_queue.back()->local());
}

Unit* ExprEvaluator::current_unit()
{
	return exec_queue.back();
}

/* Search for identifier in every possible scope, beginning from the most */
DataTable* ExprEvaluator::scope_lookup(std::string &id)
{
	if constexpr (DEBUG)
		std::cout << "Scope lookup for \"" << id << "\"" << std::endl;

	for (auto scope = std::prev(exec_queue.end());
			scope != exec_queue.begin(); --scope) {
		DataTable *local = &((*scope)->local());
		if (local->get(id) != NIL)
			return local;
	}

	auto global_scope = global();
	if (global_scope->get(id) == NIL)
		return local_scope();

	return global_scope;
}

Value& ExprEvaluator::get(std::string id)
{
	return scope_lookup(id)->get(id);
}

void ExprEvaluator::set(std::string id, Value &val)
{
	scope_lookup(id)->set(id, val);
}

inline Value ExprEvaluator::eval(Expression &expr)
{
	return expr.evaluate(*this);
}

inline Value ExprEvaluator::eval(ExprPtr expr)
{
	return eval(*expr);
}

inline void ExprEvaluator::exec(ExprPtr expr)
{
	return expr->execute(*this);
}

Value ExprEvaluator::evaluate(AssignExpr &expr)
{
	Value rhs = eval(expr.get_rhs());
	set(expr.get_identifier(), rhs);
	return rhs;
}

Value ExprEvaluator::evaluate(IdentifierExpr &expr)
{
	std::string name = expr.get_name();

	if (name == Token::reserved(Word::NEW_LINE))
		return Value(std::string(1, '\n'));

	if (name == Token::reserved(Word::NIL))
		return local_scope()->nil();

	return get(name);
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
	Value &callable = get(expr.function_name());
	if (callable.type == Type::UNIT) {
		Unit unit = callable.as<Unit>();
		Value ret = execute(unit);
		return ret;
	}

	return Value();
}

/* Default evaluation */
Value ExprEvaluator::evaluate(Expression &expr)
{
	std::cerr << "Unimplemented expression evaluation" << std::endl;
	return NIL;
}

} /* namespace mtl */
