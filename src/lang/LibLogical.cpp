#include "LibLogical.h"

#include <iterator>

#include "../ExprEvaluator.h"
#include "../structure/Value.h"
#include "../Token.h"

namespace mtl
{

void LibLogical::load()
{
	/* Logical operators: BOOL (op) BOOL */
	TokenType log_ops[] = {
			TokenType::AND, TokenType::PIPE
	};
	for (size_t i = 0; i < std::size(log_ops); ++i)
		infix_operator(log_ops[i], [=](auto lhs, auto rhs) {
			Value lexpr = val(lhs), rexpr = val(rhs);
			return Value(eval_logical(lexpr.as<bool>(), log_ops[i], rexpr.as<bool>()));
		});

	/* Logical NOT operator */
	prefix_operator(TokenType::EXCLAMATION, [this](auto rhs) {
		Value rval = val(rhs);
		return Value(!rval.as<bool>());
	});

	/* Comparison operators: >, <, >=, <= */
	TokenType log_ar_ops[] = {
			TokenType::GREATER, TokenType::GREATER_OR_EQ,
			TokenType::LESS, TokenType::LESS_OR_EQ
	};
	for (size_t i = 0; i < std::size(log_ar_ops); ++i)
		infix_operator(log_ar_ops[i], [=](auto lhs, auto rhs) {
					Value lval = val(lhs), rval = val(rhs);
					return Value(eval_logic_arithmetic(lval.as<double>(), log_ar_ops[i], rval.as<double>()));
				});

	/* Equals operator */
	infix_operator(TokenType::EQUALS, [this](auto lhs, auto rhs) {
		return Value(val(lhs) == val(rhs));
	});

	infix_operator(TokenType::NOT_EQUALS, [this](auto lhs, auto rhs) {
		return Value(val(lhs) != val(rhs));
	});
}

bool LibLogical::eval_logical(bool l, TokenType op, bool r)
{
	switch (op) {
	case TokenType::PIPE:
		return l || r;

	case TokenType::AND:
		return l && r;

	default:
		return false;
	}
}

bool LibLogical::eval_logic_arithmetic(double l, TokenType op, double r)
{
	switch (op) {
	case TokenType::GREATER:
		return l > r;

	case TokenType::LESS:
		return l < r;

	case TokenType::GREATER_OR_EQ:
		return l >= r;

	case TokenType::LESS_OR_EQ:
		return l <= r;

	default:
		return false;
	}
}

} /* namespace mtl */
