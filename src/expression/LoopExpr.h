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

		void exec_for_loop(ExprEvaluator &evaluator);
		void exec_foreach_loop(ExprEvaluator &evaluator);

		template<typename T> void for_each(ExprEvaluator &evaluator, T &container, const std::string &as_elem)
		{
			Unit &body_unit = evaluator.tmp_callable(try_cast<UnitExpr>(body).get_unit_ref());
			body_unit.call();
			DataTable &local = body_unit.local();
			evaluator.enter_scope(body_unit);
			Value &elem = local.get_or_create(as_elem);
			for (auto &val : container) {
				elem = unconst(val);
				evaluator.execute(body_unit, false);
			}
			evaluator.leave_scope();
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

		bool is_foreach()
		{
			return init != nullptr && condition != nullptr && step == nullptr;
		}

		void execute(ExprEvaluator &evaluator) override;

		std::ostream& info(std::ostream &str) override
		{
			return Expression::info(str << "{"
					<< (is_foreach() ? "For-Each" : (init != nullptr ? "For" : "While"))
					<< " Loop Expression}");
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_LOOPEXPR_H_ */
