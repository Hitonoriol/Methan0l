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

		void execute(Interpreter &context) override;
		Value evaluate(Interpreter &context) override;

		inline bool has_step()
		{
			return step != nullptr;
		}

		inline Value get_start(Interpreter &context)
		{
			return start->evaluate(context);
		}

		inline Value get_end(Interpreter &context)
		{
			return end->evaluate(context);
		}

		inline Value get_step(Interpreter &context)
		{
			if (!has_step())
				return 1;

			return step->evaluate(context);
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
