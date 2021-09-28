#include "ExprEvaluator.h"

#include "expression/AssignExpr.h"
#include "expression/ConditionalExpr.h"
#include "expression/IdentifierExpr.h"
#include "expression/BinaryOperatorExpression.h"
#include "expression/PrefixExpr.h"
#include "expression/PostfixExpr.h"

namespace mtl
{

ExprEvaluator::ExprEvaluator()
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

	/* Inline concatenation */
	binary_op(TokenType::INLINE_CONCAT, [this](auto lhs, auto rhs) {
		auto lexpr = eval(lhs), rexpr = eval(rhs);
		return Value(lexpr.to_string() + rexpr.to_string());
	});

	/* Return operator */
	postfix_op(TokenType::EXCLAMATION, [this](auto lhs) {
		auto local = local_scope();
		local->save_return(eval(lhs));
		return local->get(Unit::RETURN_KEYWORD);
	});

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
	if (!scope_queue.empty())
		scope_queue.clear();

	scope_queue.push_back(&main.local());
}

Value ExprEvaluator::execute(Unit &unit)
{
	enter_scope(unit);
	for (auto expr : unit.expressions())
		eval(expr);
	Value returned_val = unit.result();
	leave_scope();
	return returned_val;
}

Value ExprEvaluator::invoke(Function &func, ValList args)
{
	func.call(args);
	return execute(func);
}

void ExprEvaluator::enter_scope(Unit &unit)
{
	auto local = &unit.local();
	if (global() != local)
		scope_queue.push_back(local);
}

void ExprEvaluator::leave_scope()
{
	if (scope_queue.size() > 1) {
		scope_queue.back()->clear();
		scope_queue.pop_back();
	}
}

DataTable* ExprEvaluator::global()
{
	return scope_queue[0];
}

DataTable* ExprEvaluator::local_scope()
{
	return scope_queue.back();
}

/* Search for identifier in every possible scope, beginning from the most */
DataTable* ExprEvaluator::scope_lookup(std::string &id)
{
	for (auto scope = std::prev(scope_queue.end());
			scope != scope_queue.begin();
			--scope) {
		if ((*scope)->get(id) != NIL)
			return *scope;
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

	return get(name);
}

Value ExprEvaluator::evaluate(ConditionalExpr &expr)
{
	bool result = eval(expr.get_condition()).as<bool>();
	return eval(result ? expr.get_then() : expr.get_else());
}

/* Default evaluation */
Value ExprEvaluator::evaluate(Expression &expr)
{
	std::cerr << "Unimplemented expression evaluation" << '\n';
	return NIL;
}

} /* namespace mtl */
