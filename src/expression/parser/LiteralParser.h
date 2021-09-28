#ifndef SRC_EXPRESSION_PARSER_LITERALPARSER_H_
#define SRC_EXPRESSION_PARSER_LITERALPARSER_H_

#include "../../Parser.h"
#include "../Expression.h"
#include "../LiteralExpr.h"
#include "PrefixParser.h"

namespace mtl
{

class LiteralParser: public PrefixParser
{
	private:
		Type type;

	public:
		LiteralParser(Type type) : type(type) {}

		ExprPtr parse(Parser &parser, Token token) override
		{
			return ptr(new LiteralExpr(type, token));
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_LITERALPARSER_H_ */
