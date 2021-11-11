#ifndef EXPRESSION_PARSER_ASSIGNPARSER_H_
#define EXPRESSION_PARSER_ASSIGNPARSER_H_

#include "InfixParser.h"
#include "../AssignExpr.h"

namespace mtl
{

class AssignParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override
		{
			ExprPtr rhs = parser.parse(Precedence::ASSIGNMENT - 1);

			if constexpr (DEBUG)
				rhs->info(std::cout << "Assignment RHS: ") << std::endl;

			return make_expr<AssignExpr>(line(token), lhs, rhs, token.get_type() == TokenType::ARROW_R);
		}

		int precedence() override
		{
			return static_cast<int>(Precedence::ASSIGNMENT);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_ASSIGNPARSER_H_ */
