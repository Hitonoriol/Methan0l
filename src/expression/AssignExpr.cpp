#include "AssignExpr.h"

#include <ExprEvaluator.h>
#include <translator/Translator.h>
#include <iostream>

namespace mtl
{

TRANSLATE(AssignExpr)

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
			<< "{"
					<< (move ? "Move" : "Copy")
					<< " Assignment Expression // \n\t"
					<< "[" << lhs->info() << "]"
					<< " = "
					<< "[" << rhs->info() << "]"
					<< "}");
}

}
