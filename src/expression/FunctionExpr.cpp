#include "FunctionExpr.h"

#include "../type.h"
#include "UnitExpr.h"

namespace mtl
{

FunctionExpr::FunctionExpr(ArgDefList argdef, UnitExpr body) : func(argdef, body.get_unit())
{
}

Value FunctionExpr::evaluate(Interpreter &context)
{
	return Value(func);
}

void FunctionExpr::execute(Interpreter &context)
{
	ExprList args;
	context.invoke(func, args);
}

Function FunctionExpr::get_function()
{
	return func;
}

Function& FunctionExpr::function_ref()
{
	return func;
}

std::ostream& FunctionExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "Function Definition: " << BEG
					<< func
					<< END);
}

}
