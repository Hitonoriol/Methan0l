#ifndef SRC_EXPRESSION_PARSER_LITERALPARSER_H_
#define SRC_EXPRESSION_PARSER_LITERALPARSER_H_

#include "../../Parser.h"
#include "../Expression.h"
#include "../LiteralExpr.h"
#include "PrefixParser.h"

namespace mtl
{

class LiteralParser: public PrefixParser
{
	private:
		Type type;

		bool compatible(TokenType next)
		{
			return next != TokenType::INCREMENT && next != TokenType::DECREMENT;
		}

	public:
		LiteralParser(Type type) : type(type)
		{
		}

		ExprPtr parse(Parser &parser, Token token) override
		{
			if (!compatible(parser.look_ahead().get_type())) {
				if constexpr (DEBUG)
					std::cout << "Incompatible token met after the literal -- end of expression" << std::endl;
				parser.end_of_expression();
			}

			return make_expr<LiteralExpr>(line(token), type, token);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_LITERALPARSER_H_ */
