#include "MapExpr.h"

#include <map>
#include <memory>
#include <utility>

#include "../ExprEvaluator.h"
#include "../structure/DataTable.h"
#include "../structure/Value.h"
#include "../type.h"

namespace mtl
{

Value MapExpr::evaluate(ExprEvaluator &evaluator)
{
	ValMap map;

	for (auto entry : exprs)
		map.emplace(entry.first, entry.second->evaluate(evaluator));

	return Value(map);
}

void MapExpr::execute(ExprEvaluator &evaluator)
{
	ValMap map = evaluate(evaluator).get<ValMap>();
	DataTable &scope = evaluator.current_unit()->local();
	for (auto entry : map)
		scope.set(entry.first, entry.second);
}

}
