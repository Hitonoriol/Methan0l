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
		AssignExpr(ExprPtr lhs, ExprPtr rhs, bool move = false) :
				lhs(lhs), rhs(rhs), move(move)
		{
		}

		Value evaluate(ExprEvaluator &eval)
		{
			return eval.evaluate(*this);
		}

		ExprPtr get_lhs()
		{
			return lhs;
		}

		ExprPtr get_rhs()
		{
			return rhs;
		}

		bool is_move_assignment()
		{
			return move;
		}

		std::ostream& info(std::ostream &str) override
		{
			return Expression::info(str
					<< "{"
							<< (move ? "Move" : "Copy")
							<< " Assignment Expression // \n\t"
							<< "[" << lhs->info() << "]"
							<< " = "
							<< "[" << rhs->info() << "]"
							<< "}");
		}

};

} /* namespace mtl */

#endif /* EXPRESSION_ASSIGNEXPR_H_ */
