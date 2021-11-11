#include "FunctionExpr.h"

#include "../type.h"
#include "UnitExpr.h"

namespace mtl
{

FunctionExpr::FunctionExpr(ExprMap argdef, UnitExpr body) : func(argdef, body.get_unit())
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

}
