#ifndef SRC_EXPRESSION_CONDITIONALEXPR_H_
#define SRC_EXPRESSION_CONDITIONALEXPR_H_

#include "Expression.h"

namespace mtl
{

class ConditionalExpr: public Expression
{
	private:
		ExprPtr condition, then_expr, else_expr;

		ExprPtr eval_branch(Interpreter &context);

	public:
		ConditionalExpr(ExprPtr condition, ExprPtr then_expr, ExprPtr else_expr);

		ExprPtr get_condition();
		ExprPtr get_then();
		ExprPtr get_else();

		bool is_ifelse_block();

		Value evaluate(Interpreter &context) override;
		void execute(Interpreter &context) override;

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_CONDITIONALEXPR_H_ */
