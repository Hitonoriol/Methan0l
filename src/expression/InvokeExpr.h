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
		ExprPtr lhs;
		ListExpr args;

	public:
		InvokeExpr(ExprPtr lhs, ListExpr args) :
				lhs(lhs),
				args(args)
		{
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			return eval.evaluate(*this);
		}

		ExprPtr get_lhs()
		{
			return lhs;
		}

		ListExpr &arg_list()
		{
			return args;
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{Invoke Expression | args = " << args.raw_list().size() << "}";
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_INVOKEEXPR_H_ */
