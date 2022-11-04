#ifndef SRC_EXPRESSION_RANGEEXPR_H_
#define SRC_EXPRESSION_RANGEEXPR_H_

#include "Expression.h"

namespace mtl
{

class RangeExpr: public Expression
{
	private:
		ExprPtr start, end, step;

	public:
		RangeExpr(ExprPtr start, ExprPtr end, ExprPtr step);

		void execute(ExprEvaluator &evaluator) override;
		Value evaluate(ExprEvaluator &evaluator) override;

		inline bool has_step()
		{
			return step != nullptr;
		}

		inline Value get_start(ExprEvaluator &eval)
		{
			return start->evaluate(eval);
		}

		inline Value get_end(ExprEvaluator &eval)
		{
			return end->evaluate(eval);
		}

		inline Value get_step(ExprEvaluator &eval)
		{
			if (!has_step())
				return 1;

			return step->evaluate(eval);
		}

		inline ExprPtr get_start()
		{
			return start;
		}

		inline ExprPtr get_end()
		{
			return end;
		}

		inline ExprPtr get_step()
		{
			return step;
		}


		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_RANGEEXPR_H_ */
