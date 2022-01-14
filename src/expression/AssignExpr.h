#ifndef EXPRESSION_ASSIGNEXPR_H_
#define EXPRESSION_ASSIGNEXPR_H_

#include <string>

#include "../type.h"
#include "Expression.h"
#include "IdentifierExpr.h"

namespace mtl
{

class AssignExpr: public Expression
{
	private:
		ExprPtr lhs, rhs;
		bool move;

	public:
		AssignExpr(ExprPtr lhs, ExprPtr rhs, bool move = false);
		Value evaluate(ExprEvaluator &eval);

		bool is_move_assignment();

		ExprPtr get_lhs();
		ExprPtr get_rhs();

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* EXPRESSION_ASSIGNEXPR_H_ */
