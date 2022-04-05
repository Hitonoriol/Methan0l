#include "LibLogical.h"

#include <iterator>

#include "structure/Value.h"
#include "Token.h"

namespace mtl
{

void LibLogical::load()
{
	/* Logical operators: BOOL (op) BOOL */
	TokenType log_ops[] = {
			TokenType::AND, TokenType::OR, TokenType::XOR
	};
	for (size_t i = 0; i < std::size(log_ops); ++i)
		infix_operator(log_ops[i], LazyBinaryOpr([&, opr = log_ops[i]](auto lhs, auto rhs) {
			return Value(eval_logical(lhs, opr, rhs));
		}));

	/* Logical NOT operator */
	prefix_operator(TokenType::EXCLAMATION, LazyUnaryOpr([this](auto rhs) {
		Value rval = val(rhs);
		return Value(!rval.as<bool>());
	}));

	/* Bitwise operators: INT (op) INT */
	TokenType bit_ops[] = {
			TokenType::BIT_AND, TokenType::BIT_OR, TokenType::BIT_XOR,
			TokenType::SHIFT_L, TokenType::SHIFT_R
	};
	for (size_t i = 0; i < std::size(bit_ops); ++i)
		infix_operator(bit_ops[i], LazyBinaryOpr([&, opr = bit_ops[i]](auto lhs, auto rhs) {
			Value lexpr = val(lhs), rexpr = val(rhs);
			return Value(eval_bitwise(lexpr.as<dec>(), opr, rexpr.as<dec>()));
		}));

	/* Bitwise NOT */
	prefix_operator(TokenType::TILDE, LazyUnaryOpr([this](auto rhs) {
		return Value(~mtl::num(val(rhs)));
	}));

	/* Comparison operators: >, <, >=, <= */
	TokenType cmp_ops[] = {
			TokenType::GREATER, TokenType::GREATER_OR_EQ,
			TokenType::LESS, TokenType::LESS_OR_EQ
	};
	for (size_t i = 0; i < std::size(cmp_ops); ++i)
		infix_operator(cmp_ops[i], LazyBinaryOpr([&, opr = cmp_ops[i]](auto lhs, auto rhs) {
			Value lval = val(lhs), rval = val(rhs);
			return Value(eval_arithmetic_comparison(lval.as<double>(), opr, rval.as<double>()));
		}));

	/* Equals operator */
	infix_operator(TokenType::EQUALS, LazyBinaryOpr([&](auto lhs, auto rhs) {
		return Value(val(lhs) == val(rhs));
	}));

	infix_operator(TokenType::NOT_EQUALS, LazyBinaryOpr([&](auto lhs, auto rhs) {
		return Value(val(lhs) != val(rhs));
	}));
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

bool LibLogical::eval_logical(const ExprPtr &l, TokenType op, const ExprPtr &r)
{
	bool lval = bln(val(l));
	switch (op) {
	case TokenType::OR: {
		if (lval)
			return true;

		return lval || bln(val(r));
	}

	case TokenType::AND: {
		if (!lval)
			return false;

		return lval && bln(val(r));
	}

	case TokenType::XOR:
		return lval != bln(val(r));

	default:
		return false;
	}
}

bool LibLogical::eval_arithmetic_comparison(double l, TokenType op, double r)
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
