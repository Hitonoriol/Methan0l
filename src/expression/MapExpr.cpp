#include "MapExpr.h"

#include <map>
#include <memory>
#include <utility>

#include "ExprEvaluator.h"
#include "structure/DataTable.h"
#include "structure/Value.h"
#include "type.h"
#include "util/util.h"
#include "util/hash.h"

namespace mtl
{

Value MapExpr::evaluate(ExprEvaluator &evaluator)
{
	ValMap map;

	for (auto entry : exprs)
		map.emplace(Value(entry.first), entry.second->evaluate(evaluator));

	return Value(map);
}

void MapExpr::execute(ExprEvaluator &evaluator)
{
	ValMap map = evaluate(evaluator).get<ValMap>();
	DataTable &scope = evaluator.current_unit()->local();
	for (auto &entry : map) {
		std::string keystr = unconst(entry.first).to_string(&evaluator);
		scope.set(keystr, entry.second);
	}
}

std::ostream& MapExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "{"
					<< "Map Expression: " << exprs.size() << " elements"
					<< "}");
}

}
