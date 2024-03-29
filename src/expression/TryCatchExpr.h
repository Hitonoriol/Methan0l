#ifndef SRC_EXPRESSION_TRYCATCHEXPR_H_
#define SRC_EXPRESSION_TRYCATCHEXPR_H_

#include "Expression.h"

namespace mtl
{

class TryCatchExpr: public Expression
{
	private:
		ExprPtr try_body, catch_body;
		std::string catch_as;

	public:
		TryCatchExpr(ExprPtr try_body, ExprPtr catch_body, std::string &&catch_as);

		virtual void execute(Interpreter &context) override;
		virtual Value evaluate(Interpreter &context) override;

		void except(Interpreter &context);

		virtual std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_TRYCATCHEXPR_H_ */
