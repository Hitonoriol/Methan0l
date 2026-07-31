#ifndef SRC_EXPRESSION_LOOPEXPR_H_
#define SRC_EXPRESSION_LOOPEXPR_H_

#include <ch3oh/parser/expression/Expression.h>

#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/util/cast.h>
#include <methan0l/expression/UnitExpr.h>

namespace mtl
{

class LoopExpr: public Expression
{
	private:
		ExprPtr condition, body;

	public:
		LoopExpr(ExprPtr condition, ExprPtr body) : condition(condition), body(body)
		{
		}

		ExprPtr get_condition()
		{
			return condition;
		}

		ExprPtr get_body()
		{
			return body;
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_LOOPEXPR_H_ */
