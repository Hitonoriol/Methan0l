#include "ListParser.h"

#include <deque>
#include <functional>
#include <iostream>
#include <memory>

#include <lexer/Token.h>
#include <parser/Parser.h>
#include <expression/Expression.h>

namespace mtl
{

ExprPtr ListParser::parse(Parser &parser, Token token)
{
	bool as_set = token.get_type() == Tokens::SET_DEF;
	if (as_set)
		parser.consume();

	ExprList raw_list;
	parse(parser, [&](ExprPtr expr) {raw_list.push_back(expr);}, end_token);
	return make_expr<ListExpr>(line(token), raw_list, as_set);
}

/* Assumes that the `$(` token is already consumed */
void ListParser::parse(Parser &parser, std::function<void(ExprPtr)> collector, TokenType end_token)
{
	if (!parser.match(end_token)) {
		do {
			collector(parser.parse());
		} while (parser.match(Tokens::COMMA) || parser.look_ahead() != end_token);
		parser.consume(end_token);
	}
}

}
