#include "PrefixExpr.h"

#include <iostream>
#include <string>

#include "../ExprEvaluator.h"
#include "../type.h"
#include "LiteralExpr.h"

namespace mtl
{

PrefixExpr::PrefixExpr(TokenType op, ExprPtr rhs) : op(op), rhs(rhs)
{
}

TokenType PrefixExpr::get_operator()
{
	return op;
}

ExprPtr PrefixExpr::get_rhs()
{
	return rhs;
}

Value PrefixExpr::evaluate(ExprEvaluator &eval)
{
	return eval.evaluate(*this);
}

std::ostream& PrefixExpr::info(std::ostream &str)
{
	return Expression::info(str << "{Prefix Expression // \n\t"
			<< "[" << Token::to_string(op) << "] "
			<< "[" << rhs->info() << "]"
			<< "}");
}

}
