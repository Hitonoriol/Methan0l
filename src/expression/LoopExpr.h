#ifndef SRC_EXPRESSION_LOOPEXPR_H_
#define SRC_EXPRESSION_LOOPEXPR_H_

#include "Expression.h"

namespace mtl
{

class LoopExpr: public Expression
{
	private:
		ExprPtr init, condition, step;
		ExprPtr body;

		void exec_for_loop(ExprEvaluator &evaluator);
		void exec_foreach_loop(ExprEvaluator &evaluator);

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

		Value evaluate(ExprEvaluator &evaluator) override
		{
			return NIL;
		}

		bool is_foreach()
		{
			return init != nullptr && condition != nullptr && step == nullptr;
		}

		void execute(ExprEvaluator &evaluator) override;

		std::ostream& info(std::ostream &str) override
		{
			return str << "{"
					<< (is_foreach() ? "For-Each" : (init != nullptr ? "For" : "While"))
					<< " Loop Expression}";
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_LOOPEXPR_H_ */
