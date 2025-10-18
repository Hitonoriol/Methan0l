#ifndef SRC_EXPRESSION_INFIXEXPR_H_
#define SRC_EXPRESSION_INFIXEXPR_H_

#include "Expression.h"

namespace mtl
{

class InfixExpr : public Expression
{
	private:
		ExprPtr lhs;
		Token op;
		ExprPtr rhs;

	public:
		InfixExpr(ExprPtr lhs, Token opr, ExprPtr rhs);

		TokenType get_operator();
		Token get_token();
		ExprPtr& get_lhs();
		ExprPtr& get_rhs();

		std::ostream& info(std::ostream &str) override;

		EXPRESSION_IS_IMPL(InfixExpr)
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_INFIXEXPR_H_ */
