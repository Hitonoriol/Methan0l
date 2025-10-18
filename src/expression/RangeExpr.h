#ifndef SRC_EXPRESSION_RANGEEXPR_H_
#define SRC_EXPRESSION_RANGEEXPR_H_

#include <parser/expression/Expression.h>

namespace mtl
{

class RangeExpr: public Expression
{
	private:
		ExprPtr start, end, step;

	public:
		RangeExpr(ExprPtr start, ExprPtr end, ExprPtr step);

		inline bool has_step()
		{
			return step != nullptr;
		}

		Value get_start(Interpreter &context);
		Value get_end(Interpreter &context);
		Value get_step(Interpreter &context);

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
