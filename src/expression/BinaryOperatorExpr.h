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

		BinaryOperatorExpr(ExprPtr lhs,
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

			if (!Token::is_ref_opr(opr) && opr != TokenType::DOT)
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

		std::ostream& info(std::ostream &str)
		{
			return str << "{Binary Opr Expression | opr = " << static_cast<int>(opr)
					<< " (" << Token::chr(opr) << ")}";
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_BINARYOPERATOREXPR_H_ */
