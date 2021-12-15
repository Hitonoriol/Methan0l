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

		Unit& get_unit_ref()
		{
			return unit;
		}

		Value evaluate(ExprEvaluator &evaluator) override
		{
			if (unit.is_persistent() && unit.local().empty())
				evaluator.invoke(unit);

			return Value(unit);
		}

		void execute(ExprEvaluator &evaluator) override
		{
			evaluator.invoke(unit);
		}

		std::ostream& info(std::ostream &str) override
		{
			return Expression::info(str << "{Unit Expression, unit = " << unit << "}");
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_UNITEXPR_H_ */
