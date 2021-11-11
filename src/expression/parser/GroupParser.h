#ifndef EXPRESSION_PARSER_GROUPPARSER_H_
#define EXPRESSION_PARSER_GROUPPARSER_H_

#include "PrefixParser.h"

namespace mtl
{

class GroupParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token)
		{
			/* Empty group */
			if (parser.match(TokenType::PAREN_R))
				return std::make_shared<LiteralExpr>();

			ExprPtr expr = parser.parse();
			parser.consume(TokenType::PAREN_R);
			return expr;
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_GROUPPARSER_H_ */
