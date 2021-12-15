#ifndef SRC_EXPRESSION_TRYCATCHEXPR_H_
#define SRC_EXPRESSION_TRYCATCHEXPR_H_

#include <expression/Expression.h>

namespace mtl
{

class TryCatchExpr: public Expression
{
	private:
		ExprPtr try_body, catch_body;
		std::string catch_as;

	public:
		TryCatchExpr(ExprPtr try_body, ExprPtr catch_body, std::string &&catch_as);

		virtual void execute(ExprEvaluator &evaluator) override;
		virtual Value evaluate(ExprEvaluator &evaluator) override;

		void except(ExprEvaluator &evaluator);

		virtual std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_TRYCATCHEXPR_H_ */
