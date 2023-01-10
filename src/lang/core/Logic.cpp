#include <lang/core/Logic.h>

#include <lexer/Token.h>
#include <structure/Value.h>

namespace mtl
{

bool Logic::logical_operation(Interpreter &eval, const ExprPtr &l, TokenType op, const ExprPtr &r)
{
	bool lval = bln(val(eval, l));
	switch (op) {
	case TokenType::OR: {
		if (lval)
			return true;

		return lval || bln(val(eval, r));
	}

	case TokenType::AND: {
		if (!lval)
			return false;

		return lval && bln(val(eval, r));
	}

	case TokenType::XOR:
		return lval != bln(val(eval, r));

	default:
		return false;
	}
}

bool Logic::arithmetic_comparison(double l, TokenType op, double r)
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
