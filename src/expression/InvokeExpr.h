#ifndef EXPRESSION_INVOKEEXPR_H_
#define EXPRESSION_INVOKEEXPR_H_

#include <vector>

#include "Expression.h"
#include "IdentifierExpr.h"
#include "ListExpr.h"

namespace mtl
{

class InvokeExpr: public Expression
{
	private:
		IdentifierExpr identifier;
		ListExpr args;

	public:
		InvokeExpr(ExprPtr lhs, ListExpr args) :
				identifier(static_cast<IdentifierExpr&>(*lhs)),
				args(std::move(args))
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

		ListExpr arg_list()
		{
			return args;
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_INVOKEEXPR_H_ */
