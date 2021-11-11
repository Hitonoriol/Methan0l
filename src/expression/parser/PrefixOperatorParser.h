#ifndef EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_
#define EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_

#include "../../Parser.h"
#include "PrefixParser.h"
#include "../PrefixExpr.h"
#include "precedence.h"

namespace mtl
{

class PrefixOperatorParser: public PrefixParser
{
	private:
		int precedence;

	public:
		PrefixOperatorParser() : precedence(prcdc(Precedence::PREFIX))
		{
		}

		PrefixOperatorParser(int precedence) : precedence(precedence)
		{
		}

		ExprPtr parse(Parser &parser, Token token)
		{
			ExprPtr rhs = parser.parse(precedence);
			return make_expr<PrefixExpr>(line(token), token.get_type(), rhs);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_ */
