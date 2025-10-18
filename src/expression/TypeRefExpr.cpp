#include "TypeRefExpr.h"

#include <interpreter/Interpreter.h>

namespace mtl
{

TypeRefExpr::TypeRefExpr(const std::string &type_name)
	: IdentifierExpr(type_name, true) {}

} /* namespace mtl */
