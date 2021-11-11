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
		FunctionExpr(ExprMap argdef, UnitExpr body);

		Value evaluate(ExprEvaluator &evaluator) override;
		void execute(ExprEvaluator &evaluator) override;

		Function get_function();

		std::ostream& info(std::ostream &str) override
		{
			return Expression::info(str << "{Function Definition}");
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_FUNCTIONEXPR_H_ */
