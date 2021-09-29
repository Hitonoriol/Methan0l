#ifndef EXPRESSION_IDENTIFIEREXPR_H_
#define EXPRESSION_IDENTIFIEREXPR_H_

#include "Expression.h"
#include "LiteralExpr.h"
#include "../structure/Unit.h"

namespace mtl
{

class IdentifierExpr: public Expression
{
	private:
		std::string name;

	public:
		IdentifierExpr(std::string name) : name(name)
		{
		}
		;

		std::string get_name()
		{
			return name;
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			return eval.evaluate(*this);
		}

		void execute(mtl::ExprEvaluator &evaluator) override
		{
			Value val = evaluate(evaluator);
			LiteralExpr::exec_literal(evaluator, val);
		}

		inline static std::string get_name(ExprPtr expr)
		{
			return static_cast<IdentifierExpr&>(*expr).get_name();
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_IDENTIFIEREXPR_H_ */
