#include "Expression.h"

#include <memory>

#include "../Token.h"
#include "PostfixExpr.h"

namespace mtl
{

ExprPtr Expression::return_self(ExprPtr expr)
{
	return ptr(new PostfixExpr(expr, TokenType::EXCLAMATION));
}

void Expression::execute(ExprEvaluator &evaluator)
{
	evaluate(evaluator);
}

}
