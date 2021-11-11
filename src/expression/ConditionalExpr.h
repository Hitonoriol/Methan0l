#ifndef SRC_EXPRESSION_CONDITIONALEXPR_H_
#define SRC_EXPRESSION_CONDITIONALEXPR_H_

#include "Expression.h"
#include "LiteralExpr.h"

namespace mtl
{

class ConditionalExpr: public Expression
{
	private:
		ExprPtr condition, then_expr, else_expr;

	public:
		ConditionalExpr(ExprPtr condition, ExprPtr then_expr, ExprPtr else_expr) :
				condition(condition),
				then_expr(then_expr),
				else_expr(else_expr)
		{
		}

		ExprPtr get_condition()
		{
			return condition;
		}

		ExprPtr get_then()
		{
			return then_expr;
		}

		ExprPtr get_else()
		{
			return else_expr;
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			return eval.evaluate(*this);
		}

		void execute(ExprEvaluator &evaluator) override
		{
			Value val = evaluate(evaluator);
			LiteralExpr::exec_literal(evaluator, val);
		}

		std::ostream& info(std::ostream &str) override
		{
			return Expression::info(str << "{Conditional Expression}");
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_CONDITIONALEXPR_H_ */
