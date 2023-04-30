#ifndef SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_
#define SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_

#include "PrefixParser.h"

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
	public:
		ExprPtr parse(Parser &parser, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_ */
