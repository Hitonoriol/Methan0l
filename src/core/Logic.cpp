#include "Logic.h"

#include <lexer/Token.h>
#include <structure/Value.h>

namespace mtl::core
{

bool logical_operation(Interpreter &context, const ExprPtr &l, TokenType op, const ExprPtr &r)
{
	bool lval = bln(val(context, l));
	switch (op.id) {
	case Tokens::OR.id: {
		if (lval)
			return true;

		return lval || bln(val(context, r));
	}

	case Tokens::AND.id: {
		if (!lval)
			return false;

		return lval && bln(val(context, r));
	}

	case Tokens::XOR.id:
		return lval != bln(val(context, r));

	default:
		return false;
	}
}

bool arithmetic_comparison(double l, TokenType op, double r)
{
	switch (op.id) {
	case Tokens::GREATER.id:
		return l > r;

	case Tokens::LESS.id:
		return l < r;

	case Tokens::GREATER_OR_EQ.id:
		return l >= r;

	case Tokens::LESS_OR_EQ.id:
		return l <= r;

	default:
		return false;
	}
}

} /* namespace mtl */
