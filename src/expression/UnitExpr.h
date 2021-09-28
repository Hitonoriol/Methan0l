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
		UnitExpr(ExprList unit) : unit(unit) {}

		Unit get_unit()
		{
			return unit;
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_UNITEXPR_H_ */
