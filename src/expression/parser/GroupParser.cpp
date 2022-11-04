#include "GroupParser.h"

#include <expression/LiteralExpr.h>
#include <lexer/Token.h>
#include <parser/Parser.h>
#include <type.h>
#include <memory>
#include "util/cast.h"

namespace mtl
{

ExprPtr GroupParser::parse(Parser &parser, Token token)
{
	bool alt_group = token.get_type() == TokenType::INFIX_WORD_LHS_L;
	/* Empty group */
	if (parser.match(TokenType::PAREN_R)
			|| (alt_group && parser.match(TokenType::BRACKET_R)))
		return std::make_shared<LiteralExpr>();

	ExprPtr expr = parser.parse();
	parser.consume(!alt_group ? TokenType::PAREN_R : TokenType::BRACKET_R);
	return expr;
}

}
