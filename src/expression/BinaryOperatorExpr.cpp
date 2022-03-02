#include "BinaryOperatorExpr.h"

#include "../ExprEvaluator.h"
#include "../type.h"
#include "Token.h"
#include "LiteralExpr.h"

namespace mtl
{

BinaryOperatorExpr::BinaryOperatorExpr(ExprPtr lhs,
		Token opr,
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
	if (opr == TokenType::STRING_CONCAT)
		LiteralExpr::exec_literal(evaluator, val);
}

TokenType BinaryOperatorExpr::get_operator()
{
	return opr.get_type();
}

Token BinaryOperatorExpr::get_token()
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
	return Expression::info(str
			<< "Binary Operator: " << opr << " " << BEG
					<< "LHS: " << lhs->info() << NL
					<< "RHS: " << rhs->info()
					<< END);
}

bool BinaryOperatorExpr::is(Expression &expr, TokenType op)
{
	bool match = false;
	if_instanceof<BinaryOperatorExpr>(expr, [&](auto &bin) {
		match = bin.opr.get_type() == op;
	});
	return match;
}

}
