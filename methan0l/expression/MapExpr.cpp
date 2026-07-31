#include "MapExpr.h"

#include <map>
#include <memory>
#include <utility>

#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/structure/DataTable.h>
#include <methan0l/structure/Value.h>
#include <methan0l/util/util.h>
#include <methan0l/lang/util/hash.h>
#include <methan0l/CoreLibrary.h>
#include <methan0l/expression/parser/MapParser.h>

namespace mtl
{

std::ostream& MapExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "{"
					<< "Map Expression: " << exprs.size() << " elements"
					<< "}");
}

}
