#ifndef SRC_EXPRESSION_BINARYOPERATOREXPR_H_
#define SRC_EXPRESSION_BINARYOPERATOREXPR_H_

#include "Expression.h"

namespace mtl
{

class BinaryOperatorExpr: public Expression
{
	private:
		ExprPtr lhs;
		Token opr;
		ExprPtr rhs;

	public:
		TRANSLATABLE
		BinaryOperatorExpr(ExprPtr lhs, Token opr, ExprPtr rhs);

		Value evaluate(ExprEvaluator &eval) override;
		void execute(ExprEvaluator &evaluator) override;

		TokenType get_operator();
		Token get_token();
		ExprPtr& get_lhs();
		ExprPtr& get_rhs();

		std::ostream& info(std::ostream &str);
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_BINARYOPERATOREXPR_H_ */
