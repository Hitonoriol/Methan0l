#ifndef EXPRESSION_ASSIGNEXPR_H_
#define EXPRESSION_ASSIGNEXPR_H_

#include "BinaryOperatorExpr.h"

#include <string>

#include <type.h>

namespace mtl
{

class AssignExpr: public BinaryOperatorExpr
{
	private:
		bool move;

	public:
		AssignExpr(ExprPtr lhs, Token tok, ExprPtr rhs);
		Value evaluate(Interpreter &context) override;

		bool is_move_assignment();

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* EXPRESSION_ASSIGNEXPR_H_ */
