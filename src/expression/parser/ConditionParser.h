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
	private:
		static constexpr auto make_expr_block = [](UnitExpr &uexpr) {uexpr.get_unit_ref().expr_block();};

	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override
		{
			ExprPtr then_expr = parser.parse();
			ExprPtr else_expr =
					parser.match(TokenType::COLON) ?
							parser.parse(Precedence::CONDITIONAL - 1) :
							std::make_shared<LiteralExpr>();

			if_instanceof<UnitExpr>(*then_expr, make_expr_block);
			if_instanceof<UnitExpr>(*else_expr, make_expr_block);
			return make_expr<ConditionalExpr>(line(token), lhs, then_expr, else_expr);
		}

		int precedence() override
		{
			return static_cast<int>(Precedence::CONDITIONAL);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_CONDITIONPARSER_H_ */
