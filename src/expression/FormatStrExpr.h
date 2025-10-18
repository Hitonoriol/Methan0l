#ifndef SRC_EXPRESSION_FORMATSTREXPR_H_
#define SRC_EXPRESSION_FORMATSTREXPR_H_

#include <parser/expression/Expression.h>

namespace mtl
{

class FormatStrExpr: public Expression
{
	private:
		std::string fmt;
		ExprList args;

	public:
		FormatStrExpr(std::string fmt, ExprList args);

		const std::string& get_format() const;
		const ExprList& get_args() const;

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_FORMATSTREXPR_H_ */
