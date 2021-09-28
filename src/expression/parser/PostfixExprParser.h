#ifndef SRC_EXPRESSION_PARSER_POSTFIXEXPRPARSER_H_
#define SRC_EXPRESSION_PARSER_POSTFIXEXPRPARSER_H_

#include "InfixParser.h"
#include "../PostfixExpr.h"

namespace mtl
{

class PostfixExprParser: public InfixParser
{
	private:
		int opr_precedence;

	public:
		PostfixExprParser(int precedence) : opr_precedence(precedence)
		{
		}

		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override
		{
			return ptr(new PostfixExpr(lhs, token.get_type()));
		}

		int precedence() override
		{
			return opr_precedence;
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_POSTFIXEXPRPARSER_H_ */
