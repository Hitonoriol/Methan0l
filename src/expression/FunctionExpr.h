#ifndef SRC_EXPRESSION_FUNCTIONEXPR_H_
#define SRC_EXPRESSION_FUNCTIONEXPR_H_

#include "Expression.h"

namespace mtl
{

class FunctionExpr: public Expression
{
	private:
		Function func;

	public:
		FunctionExpr(ExprMap argdef, UnitExpr body) : func(argdef, body.get_unit())
		{
		}

		Value evaluate(ExprEvaluator &evaluator) override
		{
			return Value(func);
		}

		void execute(ExprEvaluator &evaluator) override
		{
			ExprList args { };
			evaluator.invoke(func, args);
		}

		Function get_function()
		{
			return func;
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{Function Expression}";
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_FUNCTIONEXPR_H_ */
