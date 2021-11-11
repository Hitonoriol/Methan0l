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
			MapExpr arg_expr = try_cast<MapExpr>(parser.parse());
			UnitExpr body_expr = try_cast<UnitExpr>(parser.parse());

			if constexpr (DEBUG)
				std::cout << "Function parser // Args: " << arg_expr.raw_map().size() << std::endl;

			return make_expr<FunctionExpr>(line(token), arg_expr.raw_map(), body_expr);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_ */
