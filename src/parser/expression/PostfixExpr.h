#ifndef SRC_EXPRESSION_POSTFIXEXPR_H_
#define SRC_EXPRESSION_POSTFIXEXPR_H_

#include "Expression.h"

namespace mtl
{

class PostfixExpr: public Expression
{
	private:
		ExprPtr lhs;
		Token op;

	public:
		PostfixExpr(ExprPtr lhs, Token op);

		ExprPtr& get_lhs();
		Token get_token();
		TokenType get_operator();

		std::ostream& info(std::ostream &str) override;

		EXPRESSION_IS_IMPL(PostfixExpr)
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_POSTFIXEXPR_H_ */
