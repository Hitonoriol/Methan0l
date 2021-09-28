#ifndef EXPRESSION_EXPRESSION_H_
#define EXPRESSION_EXPRESSION_H_

#include <any>
#include <iostream>

#include "../ExprEvaluator.h"
#include "../Token.h"
#include "../methan0l_type.h"
#include "../util.h"

namespace mtl
{

class Expression
{
	public:
		Expression() = default;
		virtual ~Expression() = default;
		virtual Value evaluate(ExprEvaluator &evaluator) = 0;
};

} /* namespace mtl */

#endif /* EXPRESSION_EXPRESSION_H_ */
