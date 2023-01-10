#include <lexer/Token.h>
#include "BinaryOperatorExpr.h"

#include "interpreter/Interpreter.h"
#include "../type.h"
#include "LiteralExpr.h"

namespace mtl
{

BinaryOperatorExpr::BinaryOperatorExpr(ExprPtr lhs,
		Token opr,
		ExprPtr rhs) :
		lhs(lhs), op(opr), rhs(rhs)
{
}

Value BinaryOperatorExpr::evaluate(Interpreter &eval)
{
	return eval.evaluate(*this);
}

void BinaryOperatorExpr::execute(Interpreter &evaluator)
{
	Value val = evaluate(evaluator);
	if (op == TokenType::STRING_CONCAT)
		LiteralExpr::exec_literal(evaluator, val);
}

TokenType BinaryOperatorExpr::get_operator()
{
	return op.get_type();
}

Token BinaryOperatorExpr::get_token()
{
	return op;
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
			<< "Binary Operator: " << op << " " << BEG
					<< "LHS: " << lhs->info() << NL
					<< "RHS: " << rhs->info()
					<< END);
}

}
