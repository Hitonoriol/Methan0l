#ifndef EXPRESSION_PARSER_INVOKEPARSER_H_
#define EXPRESSION_PARSER_INVOKEPARSER_H_

#include "../InvokeExpr.h"
#include "InfixParser.h"

namespace mtl
{

class InvokeParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override
		{
			ExprList arg_list;
			if (!parser.match(TokenType::PAREN_L)) {
				do {
					arg_list.push_back(parser.parse());
				} while (parser.match(TokenType::COMMA));
				parser.consume(TokenType::PAREN_R);
			}

			return ptr(new InvokeExpr(lhs, arg_list));
		}

		int precedence()
		{
			return static_cast<int>(Precedence::INVOKE);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_INVOKEPARSER_H_ */
