#include "UnitExpr.h"

#include <interpreter/Interpreter.h>

namespace mtl
{

UnitExpr::UnitExpr(ExprList unit, bool weak) : unit(unit, weak)
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

Value UnitExpr::evaluate(Interpreter &context)
{
	if (unit.is_persistent() && unit.local().empty())
		context.invoke(unit);

	return Value(unit);
}

void UnitExpr::execute(Interpreter &context)
{
	context.invoke(unit);
}

std::ostream& UnitExpr::info(std::ostream &str)
{
	return Expression::info(str << "Unit Expression: " << BEG << unit << END);
}

}
