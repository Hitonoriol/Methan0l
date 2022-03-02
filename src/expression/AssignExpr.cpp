#include "AssignExpr.h"

#include <ExprEvaluator.h>
#include <iostream>

namespace mtl
{

AssignExpr::AssignExpr(ExprPtr lhs, ExprPtr rhs, bool move) :
		lhs(lhs), rhs(rhs), move(move)
{
}

Value AssignExpr::evaluate(ExprEvaluator &eval)
{
	return eval.evaluate(*this);
}

ExprPtr AssignExpr::get_lhs()
{
	return lhs;
}

ExprPtr AssignExpr::get_rhs()
{
	return rhs;
}

bool AssignExpr::is_move_assignment()
{
	return move;
}

std::ostream& AssignExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< (move ? "Move" : "Copy") << " Assignment: " << BEG
					<< "LHS: " << lhs->info() << NL
					<< "RHS: " << rhs->info()
					<< END);
}

}
