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
		UnitExpr(Interpreter *context, ExprList unit, bool weak = false);

		Unit get_unit();
		Unit& get_unit_ref();

		Value evaluate(Interpreter &context) override;
		void execute(Interpreter &context) override;

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_UNITEXPR_H_ */
