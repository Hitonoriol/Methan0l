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

		void execute(Interpreter &evaluator) override;
		Value evaluate(Interpreter &evaluator) override;

		inline bool has_step()
		{
			return step != nullptr;
		}

		inline Value get_start(Interpreter &eval)
		{
			return start->evaluate(eval);
		}

		inline Value get_end(Interpreter &eval)
		{
			return end->evaluate(eval);
		}

		inline Value get_step(Interpreter &eval)
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
