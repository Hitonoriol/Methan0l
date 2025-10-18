#include "BinaryOperatorExpr.h"

#include <lexer/Token.h>
#include <interpreter/Interpreter.h>
#include <expression/LiteralExpr.h>

namespace mtl
{

BinaryOperatorExpr::BinaryOperatorExpr(ExprPtr lhs, Token opr, ExprPtr rhs)
	: InfixExpr(lhs, opr, rhs)
{
}

}
