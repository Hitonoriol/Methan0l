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
#include "CoreLibrary.h"

namespace mtl
{

Value MapExpr::evaluate(Interpreter &context)
{
	return context.make<Map>().as<Map>([&](auto &map) {
		for (auto entry : exprs)
			map->emplace(Value(entry.first), entry.second->evaluate(context));
	});;
}

void MapExpr::execute(Interpreter &context)
{
	auto &map = evaluate(context).get<Map>();
	auto &scope = context.current_unit()->local();
	for (auto &entry : map) {
		auto keystr = unconst(entry.first).to_string(&context);
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
