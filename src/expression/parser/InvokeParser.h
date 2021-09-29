#ifndef EXPRESSION_PARSER_INVOKEPARSER_H_
#define EXPRESSION_PARSER_INVOKEPARSER_H_

#include "../ListExpr.h"
#include "../InvokeExpr.h"
#include "InfixParser.h"

namespace mtl
{

class InvokeParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override
		{
			parser.emplace(token);
			ListExpr list_expr = static_cast<ListExpr&>(*parser.parse());
			return ptr(new InvokeExpr(lhs, list_expr));
		}

		int precedence()
		{
			return static_cast<int>(Precedence::INVOKE);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_INVOKEPARSER_H_ */
