#ifndef SRC_EXPRESSION_UNITEXPR_H_
#define SRC_EXPRESSION_UNITEXPR_H_

#include "Expression.h"

namespace mtl
{

class UnitExpr: public Expression
{
	private:
		Unit unit;

	public:
		UnitExpr(ExprList unit, bool weak = false) : unit(unit, weak)
		{
		}

		Unit get_unit()
		{
			return unit;
		}

		Value evaluate(ExprEvaluator &evaluator) override
		{
			return Value(unit);
		}

		void execute(ExprEvaluator &evaluator) override
		{
			evaluator.execute(unit);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_UNITEXPR_H_ */
