#include "MapExpr.h"

#include <map>
#include <memory>
#include <utility>

#include "interpreter/Interpreter.h"
#include "structure/DataTable.h"
#include "structure/Value.h"
#include "type.h"
#include "util/util.h"
#include "util/hash.h"

namespace mtl
{

Value MapExpr::evaluate(Interpreter &context)
{
	ValMap map;

	for (auto entry : exprs)
		map.emplace(Value(entry.first), entry.second->evaluate(context));

	return Value(map);
}

void MapExpr::execute(Interpreter &context)
{
	ValMap map = evaluate(context).get<ValMap>();
	DataTable &scope = context.current_unit()->local();
	for (auto &entry : map) {
		std::string keystr = unconst(entry.first).to_string(&context);
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
