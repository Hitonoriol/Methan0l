#ifndef SRC_EXPRESSION_MAPEXPR_H_
#define SRC_EXPRESSION_MAPEXPR_H_

#include "Expression.h"

namespace mtl
{

class MapExpr: public Expression
{
	private:
		ExprMap exprs;

	public:
		MapExpr(ExprMap exprs) : exprs(exprs)
		{
		}

		Value evaluate(Interpreter &context) override;

		/* Map execution works as a local idfr init list */
		void execute(Interpreter &context) override;

		ExprMap raw_map()
		{
			return exprs;
		}

		ExprMap& raw_ref()
		{
			return exprs;
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_MAPEXPR_H_ */
