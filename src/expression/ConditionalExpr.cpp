#include "ConditionalExpr.h"
#include "UnitExpr.h"

#include "translator/Translator.h"

namespace mtl
{

TRANSLATE(ConditionalExpr);

ConditionalExpr::ConditionalExpr(ExprPtr condition, ExprPtr then_expr, ExprPtr else_expr) :
		condition(condition),
		then_expr(then_expr),
		else_expr(else_expr)
{
}

ExprPtr ConditionalExpr::get_condition()
{
	return condition;
}

ExprPtr ConditionalExpr::get_then()
{
	return then_expr;
}

ExprPtr ConditionalExpr::get_else()
{
	return else_expr;
}

bool ConditionalExpr::is_ifelse_block()
{
	if (instanceof<UnitExpr>(then_expr) || instanceof<UnitExpr>(else_expr))
		return true;

	if (instanceof<ConditionalExpr>(then_expr))
		return try_cast<ConditionalExpr>(then_expr).is_ifelse_block();

	if (instanceof<ConditionalExpr>(else_expr))
		return try_cast<ConditionalExpr>(else_expr).is_ifelse_block();

	return false;
}

Value ConditionalExpr::evaluate(ExprEvaluator &eval)
{
	return eval.evaluate(*this);
}

void ConditionalExpr::execute(ExprEvaluator &evaluator)
{
	Value val = evaluate(evaluator);
	LiteralExpr::exec_literal(evaluator, val);
}

std::ostream& ConditionalExpr::info(std::ostream &str)
{
	return Expression::info(str << "{"
			<< "Conditional Expression // "
			<< "condition: " << condition->info() << " ? "
			<< "then: " << then_expr->info() << " | "
			<< "else: " << (else_expr == nullptr ? "absent" : else_expr->info())
			<< "}");
}

}
