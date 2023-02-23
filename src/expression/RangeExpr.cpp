#include "RangeExpr.h"

#include <CoreLibrary.h>

namespace mtl
{

RangeExpr::RangeExpr(ExprPtr start, ExprPtr end, ExprPtr step) :
		start(start), end(end), step(step)
{
}

void RangeExpr::execute(Interpreter &context)
{
	auto val = evaluate(context);
	out << val << NL;
}

Value RangeExpr::evaluate(Interpreter &context)
{
	auto range = context.make<List>();
	auto from = start->evaluate(context);
	auto to = end->evaluate(context);
	auto step = has_step() ? this->step->evaluate(context).as<Int>() : 1;

	return range.move_in(core::range(from, to, step, true));
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
