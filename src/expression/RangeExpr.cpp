#include "RangeExpr.h"

#include <CoreLibrary.h>

namespace mtl
{

RangeExpr::RangeExpr(ExprPtr start, ExprPtr end, ExprPtr step) :
		start(start), end(end), step(step)
{
}

Value RangeExpr::get_start(Interpreter& context)
{
	return context.evaluate(*start);
}

Value RangeExpr::get_end(Interpreter& context)
{
	return context.evaluate(*end);
}

Value RangeExpr::get_step(Interpreter& context)
{
	if (!has_step())
		return 1;

	return context.evaluate(*step);
}

std::ostream& RangeExpr::info(std::ostream &str)
{
	str << "Range: " << BEG
			<< "Start: " << start->info() << NL
			<< "End: " << end->info();

	if (has_step())
		str << NL << "Step: " << step->info();

	return Expression::info(str << END);
}

} /* namespace mtl */
