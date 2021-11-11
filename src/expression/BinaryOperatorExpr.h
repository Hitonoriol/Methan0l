#ifndef SRC_EXPRESSION_BINARYOPERATOREXPR_H_
#define SRC_EXPRESSION_BINARYOPERATOREXPR_H_

#include "Expression.h"

namespace mtl
{

class BinaryOperatorExpr: public Expression
{
	private:
		ExprPtr lhs;
		TokenType opr;
		ExprPtr rhs;

	public:

		BinaryOperatorExpr(ExprPtr lhs, TokenType opr, ExprPtr rhs);

		Value evaluate(ExprEvaluator &eval) override;
		void execute(ExprEvaluator &evaluator) override;

		TokenType get_operator();
		ExprPtr& get_lhs();
		ExprPtr& get_rhs();

		std::ostream& info(std::ostream &str);
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_BINARYOPERATOREXPR_H_ */
