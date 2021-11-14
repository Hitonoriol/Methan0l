#ifndef SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_
#define SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_

#include "PrefixParser.h"

#include "../MapExpr.h"
#include "../UnitExpr.h"
#include "../FunctionExpr.h"

namespace mtl
{

class FunctionParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			ArgDefList args;
			parser.consume(TokenType::MAP_DEF_L);
			MapParser::parse(parser, [&](auto key, auto val) {
				args.push_back(std::make_pair(key, val));
			});

			UnitExpr body_expr = try_cast<UnitExpr>(parser.parse());

			if constexpr (DEBUG)
				std::cout << "Function parser // Args: " << args.size() << std::endl;

			return make_expr<FunctionExpr>(line(token), args, body_expr);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_ */
