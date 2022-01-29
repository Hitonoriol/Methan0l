#ifndef SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_
#define SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_

#include "PrefixParser.h"

#include "../MapExpr.h"
#include "../UnitExpr.h"
#include "../FunctionExpr.h"

namespace mtl
{

/*
 * 1. func @(a, b, ...) {...}
 * 2. func: a, b, ... {...}
 * 3. #(a, b, ...) {...}
 * 4. @: a, b, ... {...}
 *
 * Or:
 * <1|2|3|4> -> expr
 */
class FunctionParser: public PrefixParser
{
	private:
		/* Re-contextualize the `->` token to stop parser from interpreting it as an infix operator */
		bool lambdize_arrow(Parser &parser);

	public:
		ExprPtr parse(Parser &parser, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_ */
