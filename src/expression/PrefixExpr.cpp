#include "PrefixExpr.h"

#include <iostream>
#include <string>

#include "interpreter/Interpreter.h"
#include "../type.h"
#include "LiteralExpr.h"

namespace mtl
{

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

Value PrefixExpr::evaluate(Interpreter &context)
{
	return context.evaluate(*this);
}

std::ostream& PrefixExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "Prefix Operator: " << op << " " << BEG
					<< "RHS: " << rhs->info()
					<< END);
}

}
