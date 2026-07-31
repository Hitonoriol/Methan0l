#include "PrefixExpr.h"

#include <iostream>
#include <string>

#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/expression/LiteralExpr.h>

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

std::ostream& PrefixExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "Prefix Operator: " << op << " " << BEG
					<< "RHS: " << rhs->info()
					<< END);
}

}
