#include "MapExpr.h"

#include <map>
#include <memory>
#include <utility>

#include <interpreter/Interpreter.h>
#include <structure/DataTable.h>
#include <structure/Value.h>
#include <util/util.h>
#include <util/hash.h>
#include <CoreLibrary.h>
#include <expression/parser/MapParser.h>

namespace mtl
{

Value MapExpr::evaluate(Interpreter &context)
{
	return context.make<Map>().as<Map>([&](auto &map) {
		for (auto entry : exprs)
			map->emplace(entry.first->evaluate(context), entry.second->evaluate(context));
	});;
}

void MapExpr::execute(Interpreter &context)
{
	auto &scope = context.current_unit()->local();
	for (auto &entry : exprs) {
		auto keystr = MapParser::key_string(entry.first);
		scope.set(keystr, entry.second->evaluate(context));
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
