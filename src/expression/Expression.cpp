#include "Expression.h"

#include "../util.h"
#include "PostfixExpr.h"

namespace mtl
{

ExprPtr Expression::return_self(ExprPtr expr)
{
	return ptr(new PostfixExpr(expr, TokenType::EXCLAMATION));
}

}
