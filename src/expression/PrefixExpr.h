#ifndef EXPRESSION_PREFIXEXPR_H_
#define EXPRESSION_PREFIXEXPR_H_

#include <functional>

#include "../Token.h"
#include "Expression.h"

namespace mtl
{

class PrefixExpr: public Expression
{
	private:
		TokenType op;
		ExprPtr rhs;

	public:
		PrefixExpr(TokenType op, ExprPtr rhs) : op(op), rhs(rhs)
		{
		}

		TokenType get_operator()
		{
			return op;
		}

		ExprPtr get_rhs()
		{
			return rhs;
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			return eval.evaluate(*this);
		}

		void execute(mtl::ExprEvaluator &evaluator) override
		{
			Value val = evaluate(evaluator);

			/* Print out list concat operator result */
			if (op == TokenType::SHIFT_L)
				LiteralExpr::exec_literal(evaluator, val);
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{Prefix Expression | opr = " << static_cast<int>(op) << "}";
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PREFIXEXPR_H_ */
