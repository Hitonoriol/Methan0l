#include "UnitExpr.h"

#include <interpreter/Interpreter.h>

namespace mtl
{

UnitExpr::UnitExpr(Interpreter *context, ExprList unit, bool weak)
	: unit(context, unit, weak)
{
}

Unit UnitExpr::get_unit()
{
	return unit;
}

Unit& UnitExpr::get_unit_ref()
{
	return unit;
}

std::ostream& UnitExpr::info(std::ostream &str)
{
	return Expression::info(str << "Unit Expression: " << BEG << unit << END);
}

}
