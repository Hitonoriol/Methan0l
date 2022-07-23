#include "FunctionParser.h"

#include <expression/Expression.h>
#include <expression/parser/MapParser.h>
#include "Parser.h"
#include <type.h>
#include <Token.h>
#include <util/cast.h>
#include <deque>
#include <iostream>
#include <memory>

namespace mtl
{

ExprPtr FunctionParser::parse(Parser &parser, Token token)
{
	/* Short form with no parentheses (2 & 4) */
	bool short_form = parser.match(TokenType::COLON)
			|| token.get_type() == TokenType::FUNC_DEF_SHORT_ALT;

	/* Parenthesized short form (3) */
	if (!short_form &&
			/* Parameter list formats: `@(a, b, ...)` or `(a, b, ...)` */
			!(parser.match(TokenType::MAP_DEF_L) || parser.match(TokenType::PAREN_L)))
		throw std::runtime_error("Invalid function definition expression");
	/* Otherwise -- parse as a regular form definition (1) */

	ArgDefList args;
	MapParser::parse(parser, [&](auto key, auto val) {
		/* Handles the no-arg case for unparenthesized forms */
		if (short_form && key == Token::reserved(Word::NIL))
			return;

		args.push_back(std::make_pair(key, val));
		LOG("* Parsed argdef pair")
	}, short_form ? TokenType::NONE : TokenType::PAREN_R);

	ExprPtr body_expr;

	/* Lambda syntax (e.g. func @(...) -> expr) */
	if (parser.look_ahead().get_type() == TokenType::ARROW_R
			&& parser.look_ahead(1).get_type() != TokenType::BRACE_L) {
		parser.consume(TokenType::ARROW_R);
		ExprList ret { Expression::return_expr(parser.parse()) };
		body_expr = make_expr<UnitExpr>(line(token), ret, false);
	}
	else
		body_expr = parser.parse();

	return make_expr<FunctionExpr>(line(token), args, try_cast<UnitExpr>(body_expr));
}

}
