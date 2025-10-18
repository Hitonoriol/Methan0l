#ifndef SRC_EXPRESSION_MAPEXPR_H_
#define SRC_EXPRESSION_MAPEXPR_H_

#include <parser/expression/Expression.h>

namespace mtl
{

class MapExpr: public Expression
{
	private:
		ExprExprMap exprs;

	public:
		MapExpr(ExprExprMap exprs)
			: exprs(exprs) {}

		ExprExprMap raw_map()
		{
			return exprs;
		}

		ExprExprMap& raw_ref()
		{
			return exprs;
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_MAPEXPR_H_ */
