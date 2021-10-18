#ifndef EXPRESSION_EXPRESSION_H_
#define EXPRESSION_EXPRESSION_H_

#include <iostream>

#include "../ExprEvaluator.h"
#include "../type.h"
#include "../structure/Value.h"

namespace mtl
{

struct Value;

class Expression
{
	public:
		Expression() = default;
		virtual ~Expression() = default;
		virtual Value evaluate(ExprEvaluator &evaluator) = 0;

		/* called instead of evaluate() for parent-less exprs */
		virtual void execute(ExprEvaluator &evaluator);

		virtual std::ostream& info(std::ostream &str)
		{
			return str << "{Expression}";
		}

		/* Create a "return" expression which returns the evaluated <expr> */
		static ExprPtr return_self(ExprPtr expr);
};

} /* namespace mtl */

#endif /* EXPRESSION_EXPRESSION_H_ */
