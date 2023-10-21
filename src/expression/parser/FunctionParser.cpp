#include "FunctionParser.h"

#include <deque>
#include <iostream>
#include <memory>

#include <expression/Expression.h>
#include <expression/parser/MapParser.h>
#include <expression/parser/UnitParser.h>
#include <expression/MapExpr.h>
#include <expression/UnitExpr.h>
#include <expression/FunctionExpr.h>
#include <lexer/Token.h>
#include <parser/Parser.h>
#include <util/cast.h>

namespace mtl
{

ExprPtr FunctionParser::parse(Parser &parser, Token token)
{
	/* Short form with no parentheses (2 & 4) */
	bool short_form = parser.match(Tokens::COLON)
			|| token.get_type() == Tokens::FUNC_DEF_SHORT_ALT;

	/* Parenthesized short form (3) */
	if (!short_form &&
			/* Parameter list formats: `@(a, b, ...)` or `(a, b, ...)` */
			!(parser.match(Tokens::MAP_DEF_L) || parser.match(Tokens::PAREN_L)))
		throw std::runtime_error("Invalid function definition expression");
	/* Otherwise -- parse as a regular form definition (1) */

	ArgDefList args;
	MapParser::parse(parser, [&](auto key, auto val) {
		auto key_str = MapParser::key_string(key);
		/* Handles the no-arg case for unparenthesized forms */
		if (short_form && key_str == ReservedWord::NIL)
			return;

		args.push_back(std::make_pair(key_str, val));
		LOG("* Parsed argdef pair")
	}, short_form ? Tokens::NONE : Tokens::PAREN_R);

	ExprPtr body_expr;

	/* Lambda syntax (e.g. `f: x, y, ... -> expr`) */
	if (parser.peek(Tokens::ARROW_R) && !parser.peek(Tokens::BRACE_L, 1)) {
		parser.consume(Tokens::ARROW_R);
		body_expr = parser.parse();
		/*
		 * Lambdas can contain either
		 * 	(1) one expression:
		 * 			`f: x, y, ... -> expr`
		 * 		In this case expr will be wrapped in a return expression
		 *
		 * 	(2) or multiple expressions separated by commas:
		 * 			`f: x, y, ... -> expr1, expr2, ...`
		 * 		Here no return expression will be created, making this yet
		 * 		another form of regular function definition
		 */
		ExprList lambda_body {
				!parser.peek(Tokens::COMMA) ?
						Expression::return_expr(body_expr) : body_expr };

		while (parser.match(Tokens::COMMA))
			lambda_body.push_back(parser.parse());

		body_expr = make_expr<UnitExpr>(line(token), lambda_body, false);
	}
	else
		body_expr = UnitParser::parse_expr_block(parser);

	auto &fbody = try_cast<UnitExpr>(body_expr);

	/* `@:` and `f:` function definition prefixes
	 * 		make resulting expression a "true" lambda (able to capture variables from upper scopes implicitly) */
	if (token.get_type() == Tokens::FUNC_DEF_SHORT || token.get_type() == Tokens::FUNC_DEF_SHORT_ALT)
		fbody.get_unit_ref().set_weak(true);

	return make_expr<FunctionExpr>(line(token), args, fbody);
}

}
