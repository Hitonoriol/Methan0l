#ifndef SRC_EXPRESSION_TRYCATCHEXPR_H_
#define SRC_EXPRESSION_TRYCATCHEXPR_H_

#include <parser/expression/Expression.h>

namespace mtl
{

class TryCatchExpr: public Expression
{
	private:
		ExprPtr try_body, catch_body;
		std::string catch_as;

	public:
		TryCatchExpr(ExprPtr try_body, ExprPtr catch_body, std::string &&catch_as);

		ExprPtr get_try() const;
		ExprPtr get_catch() const;

		void except(Interpreter &context);

		virtual std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_TRYCATCHEXPR_H_ */
