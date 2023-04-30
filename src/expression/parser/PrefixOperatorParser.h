#ifndef EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_
#define EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_

#include "PrefixParser.h"

#include <parser/Parser.h>
#include <expression/PrefixExpr.h>
#include <expression/parser/precedence.h>

namespace mtl
{

class PrefixOperatorParser: public PrefixParser
{
	public:
		using PrefixParser::PrefixParser;
		PrefixOperatorParser() : PrefixParser(Precedence::PREFIX) {}

		ExprPtr parse(Parser &parser, Token token) override
		{
			ExprPtr rhs = parser.parse(prec);
			return make_expr<PrefixExpr>(line(token), token, rhs);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_PREFIXOPERATORPARSER_H_ */
