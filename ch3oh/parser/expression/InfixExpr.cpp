#include "InfixExpr.h"

#include <ch3oh/lexer/Token.h>
#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/expression/LiteralExpr.h>

namespace mtl
{

InfixExpr::InfixExpr(ExprPtr lhs, Token opr, ExprPtr rhs)
	: lhs(lhs)
	, op(opr)
	, rhs(rhs)
{
}

TokenType InfixExpr::get_operator()
{
	return op.get_type();
}

Token InfixExpr::get_token()
{
	return op;
}

ExprPtr& InfixExpr::get_lhs()
{
	return lhs;
}

ExprPtr& InfixExpr::get_rhs()
{
	return rhs;
}

std::ostream& InfixExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "Infix expression: " << op << " " << BEG
					<< "LHS: " << lhs->info() << NL
					<< "RHS: " << rhs->info()
					<< END);
}

}
