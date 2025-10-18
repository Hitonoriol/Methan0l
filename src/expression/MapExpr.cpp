#include "MapExpr.h"

#include <map>
#include <memory>
#include <utility>

#include <interpreter/Interpreter.h>
#include <structure/DataTable.h>
#include <structure/Value.h>
#include <util/util.h>
#include <lang/util/hash.h>
#include <CoreLibrary.h>
#include <expression/parser/MapParser.h>

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
