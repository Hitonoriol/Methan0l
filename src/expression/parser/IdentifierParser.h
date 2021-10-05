#ifndef EXPRESSION_PARSER_IDENTIFIERPARSER_H_
#define EXPRESSION_PARSER_IDENTIFIERPARSER_H_

#include "../../Parser.h"
#include "../Expression.h"
#include "../IdentifierExpr.h"
#include "PrefixParser.h"

namespace mtl
{

class IdentifierParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			bool global = token.get_type() == TokenType::HASH;
			if (global)
				token = parser.consume(TokenType::IDENTIFIER);

			return ptr(new IdentifierExpr(token.get_value(), global));
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_IDENTIFIERPARSER_H_ */
