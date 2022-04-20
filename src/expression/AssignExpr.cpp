#include "AssignExpr.h"

#include <ExprEvaluator.h>
#include <iostream>

namespace mtl
{

AssignExpr::AssignExpr(ExprPtr lhs, Token tok, ExprPtr rhs) :
		BinaryOperatorExpr(lhs, tok, rhs)
{
	move = tok == TokenType::ARROW_R;
}

Value AssignExpr::evaluate(ExprEvaluator &eval)
{
	return eval.evaluate(*this);
}

bool AssignExpr::is_move_assignment()
{
	return move;
}

std::ostream& AssignExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< (move ? "Move" : "Copy") << " Assignment: " << BEG
					<< "LHS: " << get_lhs()->info() << NL
					<< "RHS: " << get_rhs()->info()
					<< END);
}

}
