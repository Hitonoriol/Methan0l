#include "LoopExpr.h"

#include <memory>
#include <stdexcept>
#include <string>

#include <util/util.h>
#include <util/meta/type_traits.h>
#include <expression/IdentifierExpr.h>
#include <expression/UnitExpr.h>
#include <structure/DataTable.h>
#include <structure/Unit.h>
#include <structure/Value.h>
#include <interpreter/Interpreter.h>
#include <CoreLibrary.h>

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
