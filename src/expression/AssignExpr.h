#ifndef EXPRESSION_ASSIGNEXPR_H_
#define EXPRESSION_ASSIGNEXPR_H_

#include <string>

#include "../methan0l_type.h"
#include "Expression.h"
#include "IdentifierExpr.h"

namespace mtl
{

class AssignExpr: public Expression
{
	private:
		ExprPtr lhs, rhs;

	public:
		AssignExpr(ExprPtr lhs, ExprPtr rhs) :
				lhs(lhs), rhs(rhs)
		{
		}

		Value evaluate(ExprEvaluator &eval)
		{
			return eval.evaluate(*this);
		}

		ExprPtr get_lhs()
		{
			return lhs;
		}

		ExprPtr get_rhs()
		{
			return rhs;
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{Assign Expression}";
		}

};

} /* namespace mtl */

#endif /* EXPRESSION_ASSIGNEXPR_H_ */
