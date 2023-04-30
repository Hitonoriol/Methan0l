#ifndef EXPRESSION_PARSER_IDENTIFIERPARSER_H_
#define EXPRESSION_PARSER_IDENTIFIERPARSER_H_

#include "PrefixParser.h"

#include <parser/Parser.h>
#include <expression/Expression.h>
#include <expression/IdentifierExpr.h>

namespace mtl
{

class IdentifierParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			bool global = token.get_type() == TokenType::HASH;
			if (global)
				token = parser.consume(TokenType::IDENTIFIER);

			auto &name = token.get_value();
			Value reserved(IdentifierExpr::eval_reserved(name));
			if (!reserved.empty())
				return make_expr<LiteralExpr>(line(token), reserved);

			return make_expr<IdentifierExpr>(line(token), name, global);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_IDENTIFIERPARSER_H_ */
