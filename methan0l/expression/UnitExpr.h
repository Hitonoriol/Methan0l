#ifndef SRC_EXPRESSION_UNITEXPR_H_
#define SRC_EXPRESSION_UNITEXPR_H_

#include <ch3oh/parser/expression/Expression.h>
#include <methan0l/structure/Unit.h>

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

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_UNITEXPR_H_ */
