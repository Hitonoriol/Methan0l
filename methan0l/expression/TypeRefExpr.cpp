#include "TypeRefExpr.h"

#include <methan0l/interpreter/Interpreter.h>

namespace mtl
{

TypeRefExpr::TypeRefExpr(const std::string &type_name)
	: IdentifierExpr(type_name, true) {}

} /* namespace mtl */
