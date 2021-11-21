#ifndef EXPRESSION_PARSER_GROUPPARSER_H_
#define EXPRESSION_PARSER_GROUPPARSER_H_

#include "PrefixParser.h"

namespace mtl
{

/*
 * (expr)
 * 	or
 * *[expr]
 */
class GroupParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token)
		{
			bool alt_group = token.get_type() == TokenType::INFIX_WORD_LHS_L;
			/* Empty group */
			if (parser.match(TokenType::PAREN_R) || (alt_group && parser.match(TokenType::BRACKET_R)))
				return std::make_shared<LiteralExpr>();

			ExprPtr expr = parser.parse();
			parser.consume(!alt_group ? TokenType::PAREN_R : TokenType::BRACKET_R);
			return expr;
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_GROUPPARSER_H_ */
