#ifndef EXPRESSION_ASSIGNEXPR_H_
#define EXPRESSION_ASSIGNEXPR_H_

#include <string>

#include "../methan0l_type.h"
#include "Expression.h"

namespace mtl
{

class AssignExpr: public Expression
{
	private:
		std::string identifier;
		ExprPtr rhs;

	public:
		AssignExpr(std::string identifier, ExprPtr rhs) :
				identifier(identifier), rhs(rhs)
		{
		}

		Value evaluate(ExprEvaluator &eval)
		{
			return eval.evaluate(*this);
		}

		std::string get_identifier()
		{
			return identifier;
		}

		ExprPtr get_rhs()
		{
			return rhs;
		}

};

} /* namespace mtl */

#endif /* EXPRESSION_ASSIGNEXPR_H_ */
