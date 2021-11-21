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
		ExprList args;

	public:
		InvokeExpr(ExprPtr lhs, ExprList args) :
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

		ExprList& arg_list()
		{
			return args;
		}

		std::ostream& info(std::ostream &str) override
		{
			return Expression::info(str << "{Invoke Expression |"
					<< " lhs: " << lhs->info()
					<< " args: " << args.size() << "}");
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_INVOKEEXPR_H_ */
