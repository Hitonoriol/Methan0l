#include "FunctionExpr.h"

#include "../type.h"
#include "UnitExpr.h"

namespace mtl
{

FunctionExpr::FunctionExpr(ArgDefList argdef, UnitExpr body) : func(argdef, body.get_unit())
{
}

Value FunctionExpr::evaluate(ExprEvaluator &evaluator)
{
	return Value(func);
}

void FunctionExpr::execute(ExprEvaluator &evaluator)
{
	ExprList args;
	evaluator.invoke(func, args);
}

Function FunctionExpr::get_function()
{
	return func;
}

Function& FunctionExpr::function_ref()
{
	return func;
}

}
