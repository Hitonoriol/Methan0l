#ifndef SRC_EXPRESSION_BINARYOPERATOREXPR_H_
#define SRC_EXPRESSION_BINARYOPERATOREXPR_H_

#include "Expression.h"

namespace mtl
{

class BinaryOperatorExpr: public Expression
{
	private:
		ExprPtr lhs;
		Token op;
		ExprPtr rhs;

	public:
		BinaryOperatorExpr(ExprPtr lhs, Token opr, ExprPtr rhs);

		Value evaluate(Interpreter &context) override;
		void execute(Interpreter &context) override;

		TokenType get_operator();
		Token get_token();
		ExprPtr& get_lhs();
		ExprPtr& get_rhs();

		std::ostream& info(std::ostream &str) override;

		_OP_EXPR_IS(BinaryOperatorExpr)
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_BINARYOPERATOREXPR_H_ */
