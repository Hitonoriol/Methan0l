#ifndef EXPRESSION_PREFIXEXPR_H_
#define EXPRESSION_PREFIXEXPR_H_

#include "Expression.h"

#include <functional>

#include <lexer/Token.h>

namespace mtl
{

class PrefixExpr: public Expression
{
	private:
		Token op;
		ExprPtr rhs;

	public:
		PrefixExpr(Token op, ExprPtr rhs);

		Token get_token();
		TokenType get_operator();
		ExprPtr get_rhs();

		Value evaluate(Interpreter &context) override;

		std::ostream& info(std::ostream &str) override;

		_OP_EXPR_IS(PrefixExpr)
};

} /* namespace mtl */

#endif /* EXPRESSION_PREFIXEXPR_H_ */
