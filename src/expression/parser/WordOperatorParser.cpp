#include "WordOperatorParser.h"

#include <memory>

#include <lexer/Token.h>
#include <parser/Parser.h>
#include <expression/Expression.h>
#include <expression/ListExpr.h>
#include <expression/PrefixExpr.h>

namespace mtl
{

/*
 * operator: expr
 * operator: expr1, expr2, ...
 */
ExprPtr WordOperatorParser::parse(Parser &parser, Token token)
{
	parser.consume(Tokens::COLON);
	ExprPtr rhs = parser.parse();
	if (multiple_args && parser.match(Tokens::COMMA)) {
		ExprList args{rhs};
		do {
			args.push_back(parser.parse());
		} while (parser.match(Tokens::COMMA));
		rhs = make_expr<ListExpr>(line(token), args);
	}

	if constexpr (DEBUG)
		out << "Word expr // " << token.to_string() << ": " << rhs->info() << std::endl;

	return make_expr<PrefixExpr>(line(token), word, rhs);
}

ExprPtr VarDefParser::parse(Parser &parser, Token token)
{
	parser.match(Tokens::COLON);
	auto rhs = parser.parse_prefix(parser.consume(Tokens::IDENTIFIER));
	return make_expr<PrefixExpr>(line(token), token.get_type(), rhs);
}

} /* namespace mtl */
