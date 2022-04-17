#ifndef SRC_EXPRESSION_LOOPEXPR_H_
#define SRC_EXPRESSION_LOOPEXPR_H_

#include "Expression.h"
#include "UnitExpr.h"
#include "util/cast.h"

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
		static inline bool loop_iteration(ExprEvaluator &eval, Unit &loop_body, Value &ret)
		{
			return !(ret = eval.execute(loop_body, false)).empty() || loop_body.execution_finished();
		}

		static inline void exit_loop(ExprEvaluator &eval, Value &ret)
		{
			eval.leave_scope();
			if (!ret.empty())
				eval.current_unit()->save_return(ret);
		}

		void exec_for_loop(ExprEvaluator &evaluator);
		void exec_foreach_loop(ExprEvaluator &evaluator);

		template<typename T> void for_each(ExprEvaluator &evaluator, T &container, const std::string &as_elem)
		{
			Unit &body_unit = evaluator.tmp_callable(Unit::from_expression(body));
			body_unit.call();
			DataTable &local = body_unit.local();
			evaluator.enter_scope(body_unit);
			Value &elem = local.get_or_create(as_elem);
			Value ret;
			for (auto &val : container) {
				elem = unconst(val);
				if (loop_iteration(evaluator, body_unit, ret))
					break;
			}
			exit_loop(evaluator, ret);
		}

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

		Value evaluate(ExprEvaluator &evaluator) override;

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

		void execute(ExprEvaluator &evaluator) override;

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_LOOPEXPR_H_ */
