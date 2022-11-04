#include <lexer/Token.h>
#include "LibLogical.h"

#include <iterator>

#include "structure/Value.h"
#include "util/array.h"

namespace mtl
{

void LibLogical::load()
{
	/* Lazy logical operators: &&, ||, ^^ */
	for_each({TokenType::AND, TokenType::OR, TokenType::XOR}, [this](auto op) {
		infix_operator(op, LazyBinaryOpr([=](auto lhs, auto rhs) {
			return Value(eval_logical(lhs, op, rhs));
		}));
	});

	/* Logical NOT operator */
	prefix_operator(TokenType::EXCLAMATION, UnaryOpr([this](Value &rhs) {
		return !rhs.as<bool>();
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
