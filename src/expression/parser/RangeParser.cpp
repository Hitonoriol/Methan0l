#include "RangeParser.h"

#include <parser/Parser.h>
#include <expression/RangeExpr.h>

namespace mtl
{

ExprPtr RangeParser::parse(Parser &parser, ExprPtr lhs, Token token)
{
	auto end = parser.parse();
	ExprPtr step = nullptr;

	/* If there is a `step` specified, current range was parsed as: start..(end..step)
	 * 		so `step` & `end` have to be unwrapped: */
	if_instanceof<RangeExpr>(*end, [&](auto &rng) {
		step = rng.get_end();
		end = rng.get_start();
	});
	return make_expr<RangeExpr>(line(token), lhs, end, step);
}

} /* namespace mtl */
