#include <expression/TryCatchExpr.h>

#include "ExceptionHandler.h"
#include "ExprEvaluator.h"
#include "util/cast.h"
#include "structure/Unit.h"
#include "expression/UnitExpr.h"
#include "util/util.h"

#include <iostream>

namespace mtl
{

TryCatchExpr::TryCatchExpr(ExprPtr try_body, ExprPtr catch_body, std::string &&catch_as) :
		try_body(try_body), catch_body(catch_body), catch_as(catch_as)
{
}

void TryCatchExpr::execute(ExprEvaluator &evaluator)
{
	evaluate(evaluator);
}

Value TryCatchExpr::evaluate(ExprEvaluator &evaluator)
{
	auto &handler = evaluator.get_exception_handler();
	handler.register_handler(evaluator.stack_depth(), this);
	Value result = evaluator.invoke(try_cast<UnitExpr>(try_body).get_unit_ref());
	handler.current_handler();
	return result;
}

void TryCatchExpr::except(ExprEvaluator &evaluator)
{
	auto &handler = evaluator.get_exception_handler();
	Value exception = handler.current_exception();
	Unit ctb = try_cast<UnitExpr>(catch_body).get_unit_ref();
	ctb.local().set(catch_as, exception);
	if constexpr (DEBUG)
		out << "Passing exception `" << exception << "` as `" << catch_as << "` to " << ctb << std::endl;
	evaluator.invoke(ctb);
}

std::ostream& TryCatchExpr::info(std::ostream &str)
{
	return Expression::info(str << "{TryCatchExpression}");
}

} /* namespace mtl */