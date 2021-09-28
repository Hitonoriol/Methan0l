#ifndef EXPRESSION_INVOKEEXPR_H_
#define EXPRESSION_INVOKEEXPR_H_

#include <vector>

#include "Expression.h"
#include "IdentifierExpr.h"

namespace mtl
{

class InvokeExpr: public Expression
{
	private:
		IdentifierExpr& identifier;
		ExprList args;

	public:
		InvokeExpr(ExprPtr lhs, ExprList args) : identifier(
				static_cast<IdentifierExpr&>(*lhs)), args(args)
		{
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			return eval.evaluate(*this);
		}

		std::string function_name()
		{
			return identifier.get_name();
		}

		ExprList arg_list()
		{
			return args;
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_INVOKEEXPR_H_ */
