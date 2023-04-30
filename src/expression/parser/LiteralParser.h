#ifndef SRC_EXPRESSION_PARSER_LITERALPARSER_H_
#define SRC_EXPRESSION_PARSER_LITERALPARSER_H_

#include "PrefixParser.h"

#include <parser/Parser.h>
#include <expression/Expression.h>
#include <expression/LiteralExpr.h>


namespace mtl
{

class LiteralParser: public PrefixParser
{
	private:
		TypeID type;

		inline bool compatible(TokenType next)
		{
			return next != TokenType::INCREMENT && next != TokenType::DECREMENT;
		}

	public:
		LiteralParser(TypeID type) : type(type)
		{}

		ExprPtr parse(Parser &parser, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_LITERALPARSER_H_ */
