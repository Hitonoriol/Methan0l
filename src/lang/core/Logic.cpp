#include <lang/core/Logic.h>

#include <lexer/Token.h>
#include <structure/Value.h>

namespace mtl::core
{

bool logical_operation(Interpreter &context, const ExprPtr &l, TokenType op, const ExprPtr &r)
{
	bool lval = bln(val(context, l));
	switch (op) {
	case TokenType::OR: {
		if (lval)
			return true;

		return lval || bln(val(context, r));
	}

	case TokenType::AND: {
		if (!lval)
			return false;

		return lval && bln(val(context, r));
	}

	case TokenType::XOR:
		return lval != bln(val(context, r));

	default:
		return false;
	}
}

bool arithmetic_comparison(double l, TokenType op, double r)
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
