#include "PostfixExpr.h"

#include <iostream>

#include <methan0l/interpreter/Interpreter.h>
#include <ch3oh/lexer/Token.h>
#include <methan0l/expression/LiteralExpr.h>

namespace mtl
{

PostfixExpr::PostfixExpr(ExprPtr lhs, Token op) : lhs(lhs), op(op)
{
}

ExprPtr& PostfixExpr::get_lhs()
{
	return lhs;
}

Token PostfixExpr::get_token()
{
	return op;
}

TokenType PostfixExpr::get_operator()
{
	return op.get_type();
}

std::ostream& PostfixExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "Postfix Operator: " << op << " " << BEG
					<< "LHS: " << lhs->info()
					<< END);
}

}
