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
		PostfixExpr(ExprPtr lhs, Token op);

		Value evaluate(Interpreter &evaluator) override;

		ExprPtr& get_lhs();
		Token get_token();
		TokenType get_operator();

		std::ostream& info(std::ostream &str) override;

		_OP_EXPR_IS(PostfixExpr)
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_POSTFIXEXPR_H_ */
