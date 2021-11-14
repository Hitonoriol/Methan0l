#ifndef EXPRESSION_PARSER_INVOKEPARSER_H_
#define EXPRESSION_PARSER_INVOKEPARSER_H_

#include "../ListExpr.h"
#include "ListParser.h"
#include "../InvokeExpr.h"
#include "InfixParser.h"

namespace mtl
{

class InvokeParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override
		{
			ExprList args;
			ListParser::parse(parser, [&](ExprPtr expr) {args.push_back(expr);});
			return make_expr<InvokeExpr>(line(token), lhs, args);
		}

		int precedence()
		{
			return prcdc(Precedence::INVOKE);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_INVOKEPARSER_H_ */
