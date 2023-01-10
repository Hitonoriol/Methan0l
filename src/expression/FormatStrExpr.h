#ifndef SRC_EXPRESSION_FORMATSTREXPR_H_
#define SRC_EXPRESSION_FORMATSTREXPR_H_

#include <expression/Expression.h>

namespace mtl
{

class FormatStrExpr: public Expression
{
	private:
		std::string fmt;
		ExprList args;

	public:
		FormatStrExpr(std::string fmt, ExprList args);

		Value evaluate(Interpreter &context) override;
		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_FORMATSTREXPR_H_ */
