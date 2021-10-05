#ifndef SRC_EXPRESSION_PARSER_CONDITIONPARSER_H_
#define SRC_EXPRESSION_PARSER_CONDITIONPARSER_H_

#include "InfixParser.h"
#include "../ConditionalExpr.h"
#include "../LiteralExpr.h"

namespace mtl
{

class ConditionParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override
		{
			ExprPtr then_expr = parser.parse();
			ExprPtr else_expr =
					parser.match(TokenType::COLON) ?
							parser.parse(Precedence::CONDITIONAL - 1) :
							ptr(new LiteralExpr());
			return ptr(new ConditionalExpr(lhs, then_expr, else_expr));
		}

		int precedence() override
		{
			return static_cast<int>(Precedence::CONDITIONAL);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_CONDITIONPARSER_H_ */
