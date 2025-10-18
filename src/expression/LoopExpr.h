#ifndef SRC_EXPRESSION_LOOPEXPR_H_
#define SRC_EXPRESSION_LOOPEXPR_H_

#include <parser/expression/Expression.h>

#include <interpreter/Interpreter.h>
#include <util/cast.h>
#include <expression/UnitExpr.h>

namespace mtl
{

class LoopExpr: public Expression
{
	private:
		ExprPtr init, condition, step;
		ExprPtr body;

	public:
		/* do $(i = 0, i < 10, ++i) -> {expr1; expr2; expr3} */
		LoopExpr(ExprPtr init, ExprPtr condition, ExprPtr step, ExprPtr body) :
				init(init), condition(condition), step(step), body(body)
		{
		}

		LoopExpr(ExprList params, ExprPtr body) : LoopExpr(params[0], params[1], params[2], body)
		{
		}

		/* do $(as_elem, list) -> {} */
		LoopExpr(ExprPtr elem_name, ExprPtr list, ExprPtr body) :
				init(elem_name), condition(list), body(body)
		{
		}

		/* do $(i < 10) -> {expr1; expr2; expr3} */
		LoopExpr(ExprPtr condition, ExprPtr body) : LoopExpr(nullptr, condition, nullptr, body)
		{
		}

		ExprPtr get_init()
		{
			return init;
		}

		ExprPtr get_condition()
		{
			return condition;
		}

		ExprPtr get_step()
		{
			return step;
		}

		ExprPtr get_body()
		{
			return body;
		}

		inline bool is_foreach()
		{
			return init != nullptr && condition != nullptr && step == nullptr;
		}

		inline bool is_while()
		{
			return !is_foreach() && init == nullptr;
		}

		inline bool is_for()
		{
			return !is_foreach() && !is_while();
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_LOOPEXPR_H_ */
