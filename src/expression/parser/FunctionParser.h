#ifndef SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_
#define SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_

#include "PrefixParser.h"

#include "../MapExpr.h"
#include "../UnitExpr.h"
#include "../FunctionExpr.h"

namespace mtl
{

/*
 * func @(a, b, ...) {...}
 * #(a, b, ...) {...}
 */
class FunctionParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			if (!parser.match(TokenType::MAP_DEF_L))
				token.assert_type(TokenType::FUNC_DEF_SHORT);

			ArgDefList args;
			MapParser::parse(parser, [&](auto key, auto val) {
				args.push_back(std::make_pair(key, val));
			});

			UnitExpr body_expr = try_cast<UnitExpr>(parser.parse());

			return make_expr<FunctionExpr>(line(token), args, body_expr);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_ */
