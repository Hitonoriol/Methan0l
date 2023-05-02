#include "GroupParser.h"

#include <memory>

#include <lexer/Token.h>
#include <parser/Parser.h>
#include <expression/LiteralExpr.h>
#include <expression/TypeRefExpr.h>
#include <util/cast.h>

namespace mtl
{

ExprPtr GroupParser::parse(Parser &parser, Token token)
{
	/* Alternative group syntax: *[exprs] */
	bool alt_group = token.get_type() == TokenType::INFIX_WORD_LHS_L;
	/* Empty group */
	if (parser.match(TokenType::PAREN_R)
			|| (alt_group && parser.match(TokenType::BRACKET_R)))
		return std::make_shared<LiteralExpr>();

	/* Type reference expression: (TypeName) */
	else if (parser.peek(TokenType::IDENTIFIER) && parser.peek(TokenType::PAREN_R, 1)) {
		auto type_tok = parser.consume(TokenType::IDENTIFIER);
		parser.consume(TokenType::PAREN_R);
		return make_expr<TypeRefExpr>(line(token), type_tok.get_value());
	}

	auto expr = parser.parse();
	parser.consume(!alt_group ? TokenType::PAREN_R : TokenType::BRACKET_R);
	return expr;
}

}
