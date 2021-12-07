#include "ListExpr.h"

#include <deque>

#include "../ExprEvaluator.h"
#include "../type.h"
#include "../structure/Value.h"

namespace mtl
{

ListExpr::ListExpr(ExprList exprs, bool as_set) : exprs(exprs), as_set(as_set)
{
}

ExprList& ListExpr::raw_list()
{
	return exprs;
}

Value ListExpr::evaluate(ExprEvaluator &evaluator)
{
	return as_set ? create_and_populate<ValSet>(evaluator) : create_and_populate<ValList>(evaluator);
}

std::ostream& ListExpr::info(std::ostream &str)
{
	return Expression::info(str << "{List Expression // " << exprs.size() << " elements}");
}

}
