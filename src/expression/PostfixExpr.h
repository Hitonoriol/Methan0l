#ifndef SRC_EXPRESSION_POSTFIXEXPR_H_
#define SRC_EXPRESSION_POSTFIXEXPR_H_

#include "Expression.h"
#include "LiteralExpr.h"

namespace mtl
{

class PostfixExpr: public Expression
{
	private:
		ExprPtr lhs;
		Token op;

	public:
		TRANSLATABLE
		PostfixExpr(ExprPtr lhs, Token op);

		Value evaluate(ExprEvaluator &evaluator) override;

		ExprPtr& get_lhs();
		Token get_token();
		TokenType get_operator();

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_POSTFIXEXPR_H_ */
