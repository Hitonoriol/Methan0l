#include "PostfixExpr.h"

#include <iostream>

#include <interpreter/Interpreter.h>
#include <lexer/Token.h>
#include <expression/LiteralExpr.h>

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
