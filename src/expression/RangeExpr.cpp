#include "RangeExpr.h"
#include "lang/core/Data.h"

namespace mtl
{

RangeExpr::RangeExpr(ExprPtr start, ExprPtr end, ExprPtr step) :
		start(start), end(end), step(step)
{
}

void RangeExpr::execute(ExprEvaluator &evaluator)
{
	auto val = evaluate(evaluator);
	out << val << NL;
}

Value RangeExpr::evaluate(ExprEvaluator &evaluator)
{
	return Data::range(start->evaluate(evaluator),
			end->evaluate(evaluator),
			has_step() ? step->evaluate(evaluator).as<dec>() : 1,
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
