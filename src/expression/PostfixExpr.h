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
		TokenType op;

	public:
		PostfixExpr(ExprPtr lhs, TokenType op) : lhs(lhs), op(op)
		{
		}

		Value evaluate(ExprEvaluator &evaluator) override
		{
			return evaluator.evaluate(*this);
		}

		ExprPtr& get_lhs()
		{
			return lhs;
		}

		TokenType get_operator()
		{
			return op;
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{Postfix Expression | opr = " << static_cast<int>(op) << "}";
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_POSTFIXEXPR_H_ */
