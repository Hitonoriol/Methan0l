#ifndef SRC_EXPRESSION_LOOPEXPR_H_
#define SRC_EXPRESSION_LOOPEXPR_H_

#include "Expression.h"

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

		/* Perform one loop iteration
		 *
		 * Assigns `loop_body`'s return value to `ret` if a return has occurred.
		 * returns `true` if loop was interrupted, `false` otherwise.
		 */
		static inline bool loop_iteration(Interpreter &context, Unit &loop_body, Value &ret)
		{
			return !(ret = context.execute(loop_body, false)).empty() || loop_body.execution_finished();
		}

		static inline void exit_loop(Interpreter &context, Value &ret)
		{
			context.leave_scope();
			if (!ret.empty())
				context.current_unit()->save_return(ret);
		}

		void exec_for_loop(Interpreter &context);
		void exec_foreach_loop(Interpreter &context);

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

		Value evaluate(Interpreter &context) override;

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

		void execute(Interpreter &context) override;

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_LOOPEXPR_H_ */
