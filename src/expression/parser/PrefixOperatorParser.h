#ifndef EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_
#define EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_

#include <parser/Parser.h>
#include "PrefixParser.h"
#include "../PrefixExpr.h"
#include "precedence.h"

namespace mtl
{

class PrefixOperatorParser: public PrefixParser
{
	public:
		PrefixOperatorParser() : PrefixParser(prcdc(Precedence::PREFIX))
		{
		}

		PrefixOperatorParser(int precedence) : PrefixParser(precedence)
		{
		}

		ExprPtr parse(Parser &parser, Token token)
		{
			ExprPtr rhs = parser.parse(prec);
			return make_expr<PrefixExpr>(line(token), token, rhs);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_ */
