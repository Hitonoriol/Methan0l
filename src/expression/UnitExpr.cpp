#include "UnitExpr.h"

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

Value UnitExpr::evaluate(ExprEvaluator &evaluator)
{
	if (unit.is_persistent() && unit.local().empty())
		evaluator.invoke(unit);

	return Value(unit);
}

void UnitExpr::execute(ExprEvaluator &evaluator)
{
	evaluator.invoke(unit);
}

std::ostream& UnitExpr::info(std::ostream &str)
{
	return Expression::info(str << "Unit Expression: " << BEG << unit << END);
}

}
