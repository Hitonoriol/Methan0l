#include "RangeParser.h"

#include <ch3oh/parser/Parser.h>
#include <methan0l/expression/RangeExpr.h>

namespace mtl
{

ExprPtr RangeParser::parse(Parser &parser, ExprPtr lhs, Token token)
{
	/* Keep lower-precedence expressions (notably pipelines) outside the range,
	 * while still allowing the second `..` in start..end..step. */
	auto end = parser.parse(precedence() - 1);
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
