#include "TypeRefExpr.h"

#include <interpreter/Interpreter.h>

namespace mtl
{

TypeRefExpr::TypeRefExpr(const std::string &type_name)
	: IdentifierExpr(type_name, true) {}

Value TypeRefExpr::evaluate(Interpreter &context)
{
	return context.get_type_mgr().get_type(name).type_id();
}

} /* namespace mtl */
