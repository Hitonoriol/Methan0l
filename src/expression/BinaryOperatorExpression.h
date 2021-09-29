#ifndef SRC_EXPRESSION_BINARYOPERATOREXPRESSION_H_
#define SRC_EXPRESSION_BINARYOPERATOREXPRESSION_H_

#include "Expression.h"

namespace mtl
{

class BinaryOperatorExpression: public Expression
{
	private:
		ExprPtr lhs;
		TokenType opr;
		ExprPtr rhs;

	public:

		BinaryOperatorExpression(ExprPtr lhs,
				TokenType opr,
				ExprPtr rhs) :
				lhs(lhs), opr(opr), rhs(rhs)
		{
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			return eval.evaluate(*this);
		}

		void execute(ExprEvaluator &evaluator) override
		{
			Value val = evaluate(evaluator);
			LiteralExpr::exec_literal(evaluator, val);
		}

		TokenType get_operator()
		{
			return opr;
		}

		ExprPtr& get_lhs()
		{
			return lhs;
		}

		ExprPtr& get_rhs()
		{
			return rhs;
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_BINARYOPERATOREXPRESSION_H_ */
