#ifndef SRC_EXPRESSION_FUNCTIONEXPR_H_
#define SRC_EXPRESSION_FUNCTIONEXPR_H_

#include <parser/expression/Expression.h>
#include <structure/Function.h>

namespace mtl
{

class UnitExpr;

class FunctionExpr: public Expression
{
	private:
		Function func;

	public:
		FunctionExpr(ArgDefList argdef, UnitExpr body);

		Function get_function();
		Function& function_ref();

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_FUNCTIONEXPR_H_ */
