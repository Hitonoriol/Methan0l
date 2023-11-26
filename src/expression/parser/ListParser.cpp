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
	auto type = token.get_type();
	bool as_set = type == Tokens::SET_DEF;
	bool keyword_form = as_set || type == Tokens::LIST_DEF;
	auto end_token = keyword_form ? Tokens::NONE : this->end_token;
	if (keyword_form)
		parser.consume(Tokens::COLON);

	ExprList raw_list;
	parse(parser, [&](ExprPtr expr) {raw_list.push_back(expr);}, end_token);
	return make_expr<ListExpr>(line(token), raw_list, as_set);
}

/* Parses a comma-separated list of expressions.
 * Assumes that the start token (if any) is already consumed.
 */
void ListParser::parse(Parser &parser, std::function<void(ExprPtr)> collector, TokenType end_token)
{
	if (parser.match(end_token)) 
		return;

	do {
		collector(parser.parse());
	} while (parser.match(Tokens::COMMA) && parser.look_ahead() != end_token);

	if (end_token != Tokens::NONE) {
		parser.consume(end_token);
	}
}

}
