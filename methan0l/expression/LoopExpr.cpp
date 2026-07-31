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
	bool lforeach = is_foreach(), lfor = is_for();
	return Expression::info(str
					<< (lforeach ? "For-Each" : (lfor ? "For" : "While"))
					<< " Loop: " << BEG
					<< (lforeach || lfor ? ("Init: " + init->info() + mtl::str(NL)) : "")
					<< "Condition: " << condition->info() << NL
					<< (lfor ? ("Step: " + step->info() + mtl::str(NL)) : "")
					<< "Body: " << body->info() << NL
					<< END);
}

}
