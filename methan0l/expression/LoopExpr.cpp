#include "LoopExpr.h"

#include <memory>
#include <stdexcept>
#include <string>

#include <methan0l/util/util.h>
#include <methan0l/util/meta/type_traits.h>
#include <methan0l/expression/IdentifierExpr.h>
#include <methan0l/expression/UnitExpr.h>
#include <methan0l/structure/DataTable.h>
#include <methan0l/structure/Unit.h>
#include <methan0l/structure/Value.h>
#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/CoreLibrary.h>

namespace mtl
{

std::ostream& LoopExpr::info(std::ostream &str)
{
	return Expression::info(str
					<< "While Loop: " << BEG
					<< "Condition: " << condition->info() << NL
					<< "Body: " << body->info() << NL
					<< END);
}

}
