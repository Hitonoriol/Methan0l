#ifndef EXPRESSION_EXPRESSION_H_
#define EXPRESSION_EXPRESSION_H_

#include <iostream>

#include "../ExprEvaluator.h"
#include "../methan0l_type.h"
#include "../structure/Value.h"

namespace mtl
{

class Expression
{
	public:
		Expression() = default;
		virtual ~Expression() = default;
		virtual Value evaluate(ExprEvaluator &evaluator) = 0;

		/* called instead of evaluate() for parent-less exprs */
		virtual void execute(ExprEvaluator &evaluator)
		{
			evaluate(evaluator);
		}

		virtual std::ostream& info(std::ostream &str)
		{
			return str << "{Expression}";
		}

		/* Create a "return" expression which returns the evaluated <expr> */
		static ExprPtr return_self(ExprPtr expr);
};

} /* namespace mtl */

#endif /* EXPRESSION_EXPRESSION_H_ */
