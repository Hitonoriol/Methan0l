#ifndef EXPRESSION_PREFIXEXPR_H_
#define EXPRESSION_PREFIXEXPR_H_

#include <functional>

#include "../Token.h"
#include "Expression.h"

namespace mtl
{

class PrefixExpr: public Expression
{
	private:
		TokenType op;
		ExprPtr rhs;

	public:
		PrefixExpr(TokenType op, ExprPtr rhs);

		TokenType get_operator();
		ExprPtr get_rhs();

		Value evaluate(ExprEvaluator &eval) override;

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* EXPRESSION_PREFIXEXPR_H_ */
