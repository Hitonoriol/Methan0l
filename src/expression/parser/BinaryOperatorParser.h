#ifndef SRC_EXPRESSION_PARSER_BINARYOPERATORPARSER_H_
#define SRC_EXPRESSION_PARSER_BINARYOPERATORPARSER_H_

#include "../BinaryOperatorExpr.h"
#include "InfixParser.h"

namespace mtl
{

class BinaryOperatorParser: public InfixParser
{
	private:
		int opr_precedence;
		bool right_assoc;

	public:
		BinaryOperatorParser(int precedence, bool is_right) : opr_precedence(precedence), right_assoc(is_right)
		{
		}

		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override
		{
			ExprPtr rhs = parser.parse(opr_precedence - (right_assoc ? 1 : 0));
			return make_expr<BinaryOperatorExpr>(line(token), lhs, token.get_type(), rhs);
		}

		int precedence() override
		{
			return opr_precedence;
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_BINARYOPERATORPARSER_H_ */
