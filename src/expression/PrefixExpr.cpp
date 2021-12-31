#include "PrefixExpr.h"

#include <iostream>
#include <string>

#include "../ExprEvaluator.h"
#include "../type.h"
#include "LiteralExpr.h"
#include "translator/Translator.h"

namespace mtl
{

TRANSLATE(PrefixExpr)

PrefixExpr::PrefixExpr(Token op, ExprPtr rhs) : op(op), rhs(rhs)
{
}

TokenType PrefixExpr::get_operator()
{
	return op.get_type();
}

Token PrefixExpr::get_token()
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
			<< "[" << op << "] "
			<< "[" << rhs->info() << "]"
			<< "}");
}

}
