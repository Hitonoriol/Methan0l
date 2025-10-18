#ifndef SRC_EXPRESSION_BINARYOPERATOREXPR_H_
#define SRC_EXPRESSION_BINARYOPERATOREXPR_H_

#include <parser/expression/InfixExpr.h>

namespace mtl
{

class BinaryOperatorExpr : public InfixExpr
{
	public:
		BinaryOperatorExpr(ExprPtr lhs, Token opr, ExprPtr rhs);

		EXPRESSION_IS_IMPL(BinaryOperatorExpr)
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_BINARYOPERATOREXPR_H_ */
