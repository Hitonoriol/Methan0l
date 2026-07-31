#include "BinaryOperatorExpr.h"

#include <ch3oh/lexer/Token.h>
#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/expression/LiteralExpr.h>

namespace mtl
{

BinaryOperatorExpr::BinaryOperatorExpr(ExprPtr lhs, Token opr, ExprPtr rhs)
	: InfixExpr(lhs, opr, rhs)
{
}

}
