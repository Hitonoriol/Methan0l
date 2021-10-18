#include "ListExpr.h"

#include <deque>

#include "../ExprEvaluator.h"
#include "../type.h"
#include "../structure/Value.h"

namespace mtl
{

Value ListExpr::evaluate(ExprEvaluator &evaluator)
{
	ValList list;

	for (auto expr : exprs)
		list.push_back(expr->evaluate(evaluator));

	return Value(list);
}

}
