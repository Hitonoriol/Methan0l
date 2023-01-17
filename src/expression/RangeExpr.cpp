#include "RangeExpr.h"
#include "lang/core/Data.h"

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
	return core::range(start->evaluate(context),
			end->evaluate(context),
			has_step() ? step->evaluate(context).as<dec>() : 1,
			true);
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
