#ifndef SRC_EXPRESSION_PARSER_LITERALPARSER_H_
#define SRC_EXPRESSION_PARSER_LITERALPARSER_H_

#include <ch3oh/parser/expression/PrefixExprParser.h>

#include <ch3oh/parser/Parser.h>
#include <ch3oh/parser/expression/Expression.h>
#include <methan0l/expression/LiteralExpr.h>


namespace mtl
{

class LiteralParser: public PrefixParser
{
	private:
		TypeID type;

		inline bool compatible(TokenType next)
		{
			return next != Tokens::INCREMENT && next != Tokens::DECREMENT;
		}

	public:
		LiteralParser(TypeID type) : type(type)
		{}

		ExprPtr parse(Parser &parser, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_LITERALPARSER_H_ */
