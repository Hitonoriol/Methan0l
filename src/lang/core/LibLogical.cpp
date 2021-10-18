#include "LibLogical.h"

#include <iterator>

#include "../../structure/Value.h"
#include "../../Token.h"

namespace mtl
{

void LibLogical::load()
{
	/* Logical operators: BOOL (op) BOOL */
	TokenType log_ops[] = {
			TokenType::AND, TokenType::OR, TokenType::XOR
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

	/* Bitwise operators: INT (op) INT */
	TokenType bit_ops[] = {
			TokenType::BIT_AND, TokenType::BIT_OR, TokenType::BIT_XOR,
			TokenType::SHIFT_L, TokenType::SHIFT_R
	};
	for (size_t i = 0; i < std::size(bit_ops); ++i)
		infix_operator(bit_ops[i], [=](auto lhs, auto rhs) {
			Value lexpr = val(lhs), rexpr = val(rhs);
			return Value(eval_bitwise(lexpr.as<dec>(), bit_ops[i], rexpr.as<dec>()));
		});

	/* Bitwise NOT */
	prefix_operator(TokenType::TILDE, [this](auto rhs) {
		return Value(~mtl::num(val(rhs)));
	});

	/* Comparison operators: >, <, >=, <= */
	TokenType log_ar_ops[] = {
			TokenType::GREATER, TokenType::GREATER_OR_EQ,
			TokenType::LESS, TokenType::LESS_OR_EQ
	};
	for (size_t i = 0; i < std::size(log_ar_ops); ++i)
		infix_operator(log_ar_ops[i], [=](auto lhs,
				auto rhs) {
					Value lval = val(lhs), rval = val(rhs);
					return Value(eval_logic_arithmetic(lval.as<double>(), log_ar_ops[i], rval.as<double>()));
				});

	/* Equals operator */
	infix_operator(TokenType::EQUALS, [&](auto lhs, auto rhs) {
		return Value(val(lhs) == val(rhs));
	});

	infix_operator(TokenType::NOT_EQUALS, [&](auto lhs, auto rhs) {
		return Value(val(lhs) != val(rhs));
	});
}

dec LibLogical::eval_bitwise(dec l, TokenType op, dec r)
{
	switch (op) {
	case TokenType::BIT_OR:
		return l | r;

	case TokenType::BIT_AND:
		return l & r;

	case TokenType::BIT_XOR:
		return l ^ r;

	case TokenType::SHIFT_L:
		return l << r;

	case TokenType::SHIFT_R:
		return l >> r;

	default:
		return 0;
	}
}

bool LibLogical::eval_logical(bool l, TokenType op, bool r)
{
	switch (op) {
	case TokenType::OR:
		return l || r;

	case TokenType::AND:
		return l && r;

	case TokenType::XOR:
		return l != r;

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
