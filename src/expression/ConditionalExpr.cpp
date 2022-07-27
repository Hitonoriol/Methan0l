#include "ConditionalExpr.h"
#include "UnitExpr.h"

namespace mtl
{

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

ExprPtr ConditionalExpr::eval_branch(ExprEvaluator &eval)
{
	return (condition->evaluate(eval).to_bool() ? then_expr : else_expr);
}

Value ConditionalExpr::evaluate(ExprEvaluator &eval)
{
	return eval_branch(eval)->evaluate(eval);
}

void ConditionalExpr::execute(ExprEvaluator &evaluator)
{
	eval_branch(evaluator)->execute(evaluator);
}

std::ostream& ConditionalExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "Conditional Expression: " << BEG
					<< "Condition: " << condition->info() << NL
					<< "Then: " << then_expr->info() << NL
					<< "Else: " << (else_expr == nullptr ? "absent" : else_expr->info()) << NL
					<< END);
}

}
