#ifndef EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_
#define EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_

#include "PrefixParser.h"
#include "../PrefixExpr.h"

namespace mtl
{

class PrefixOperatorParser: public PrefixParser
{
	private:
		int precedence;

	public:
		PrefixOperatorParser(int precedence) : precedence(precedence)
		{
		}

		ExprPtr parse(Parser &parser, Token token)
		{
			ExprPtr rhs = parser.parse(precedence);
			return ptr(new PrefixExpr(token.get_type(), rhs));
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_ */
