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
		FunctionExpr(ArgDefList argdef, UnitExpr body);

		Value evaluate(Interpreter &evaluator) override;
		void execute(Interpreter &evaluator) override;

		Function get_function();
		Function& function_ref();

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_FUNCTIONEXPR_H_ */
