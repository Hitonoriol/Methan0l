#ifndef EXPRESSION_PREFIXEXPR_H_
#define EXPRESSION_PREFIXEXPR_H_

#include <functional>

#include "../Token.h"
#include "Expression.h"

namespace mtl
{

class PrefixExpr: public Expression
{
	private:
		TokenType op;
		ExprPtr rhs;

	public:
		PrefixExpr(TokenType op, ExprPtr rhs) : op(op), rhs(rhs)
		{
		}

		TokenType get_operator()
		{
			return op;
		}

		ExprPtr& get_rhs()
		{
			return rhs;
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			PrefixOpr opr_applier;
			return eval.evaluate(*this);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PREFIXEXPR_H_ */
