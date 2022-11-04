#include "PostfixExpr.h"

#include <interpreter/ExprEvaluator.h>
#include <lexer/Token.h>
#include <type.h>
#include <iostream>


namespace mtl
{

PostfixExpr::PostfixExpr(ExprPtr lhs, Token op) : lhs(lhs), op(op)
{
}

Value PostfixExpr::evaluate(ExprEvaluator &evaluator)
{
	return evaluator.evaluate(*this);
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
