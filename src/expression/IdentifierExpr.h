#ifndef EXPRESSION_IDENTIFIEREXPR_H_
#define EXPRESSION_IDENTIFIEREXPR_H_

#include "Expression.h"

namespace mtl
{

class IdentifierExpr: public Expression
{
	private:
		std::string name;

	public:
		IdentifierExpr(std::string name) : name(name) {};

		std::string get_name()
		{
			return name;
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			return eval.evaluate(*this);
		}

		inline static std::string get_name(ExprPtr expr)
		{
			return static_cast<IdentifierExpr&>(*expr).get_name();
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_IDENTIFIEREXPR_H_ */
