#include "ListParser.h"

#include <deque>
#include <functional>
#include <iostream>
#include <memory>

#include "../../Parser.h"
#include "../../type.h"
#include "../../Token.h"
#include "../Expression.h"

namespace mtl
{

ExprPtr ListParser::parse(Parser &parser, Token token)
{
	bool as_set = token.get_type() == TokenType::SET_DEF;
	if (as_set)
		parser.consume(TokenType::LIST_DEF_L);

	ExprList raw_list;
	parse(parser, [&](ExprPtr expr) {raw_list.push_back(expr);});
	return make_expr<ListExpr>(line(token), raw_list, as_set);
}

/* Assumes that the `$(` token is already consumed */
void ListParser::parse(Parser &parser, std::function<void(ExprPtr)> collector)
{
	if (!parser.match(TokenType::PAREN_R)) {
		do {
			collector(parser.parse());
		} while (parser.match(TokenType::COMMA) || parser.look_ahead() != TokenType::PAREN_R);
		parser.consume(TokenType::PAREN_R);
	}
}

}
