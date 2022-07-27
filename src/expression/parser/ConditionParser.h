#ifndef SRC_EXPRESSION_PARSER_CONDITIONPARSER_H_
#define SRC_EXPRESSION_PARSER_CONDITIONPARSER_H_

#include "InfixParser.h"
#include "../ConditionalExpr.h"
#include "../LiteralExpr.h"
#include "../UnitExpr.h"

namespace mtl
{

class ConditionParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override;

		int precedence() override
		{
			return static_cast<int>(Precedence::CONDITIONAL);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_CONDITIONPARSER_H_ */
