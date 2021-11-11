#include "BinaryOperatorExpr.h"

#include "../ExprEvaluator.h"
#include "../type.h"
#include "../Token.h"
#include "LiteralExpr.h"

namespace mtl
{

BinaryOperatorExpr::BinaryOperatorExpr(ExprPtr lhs,
		TokenType opr,
		ExprPtr rhs) :
		lhs(lhs), opr(opr), rhs(rhs)
{
}

Value BinaryOperatorExpr::evaluate(ExprEvaluator &eval)
{
	return eval.evaluate(*this);
}

void BinaryOperatorExpr::execute(ExprEvaluator &evaluator)
{
	Value val = evaluate(evaluator);

	if (!Token::is_ref_opr(opr) && opr != TokenType::DOT)
		LiteralExpr::exec_literal(evaluator, val);
}

TokenType BinaryOperatorExpr::get_operator()
{
	return opr;
}

ExprPtr& BinaryOperatorExpr::get_lhs()
{
	return lhs;
}

ExprPtr& BinaryOperatorExpr::get_rhs()
{
	return rhs;
}

std::ostream& BinaryOperatorExpr::info(std::ostream &str)
{
	return Expression::info(str << "{Binary Opr Expression // \n\t"
			<< "[" << lhs->info() << "] "
			<< "[" << Token::to_string(opr) << "]"
			<< " [" << rhs->info() << "]"
			<< "}");
}

}
