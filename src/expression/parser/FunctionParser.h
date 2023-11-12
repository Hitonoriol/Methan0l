#ifndef SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_
#define SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_

#include "PrefixParser.h"

namespace mtl
{

/*
 * Syntax:
 * 1. func: a, b, ... {...}
 * 2. f: a, b, ... {...}
 * 
 * No args:
 * 1. func: () {...}
 * 2. f: () {...}
 * 
 * Short form:
 * 1. f: a, b, ... -> ...
 * 2. f: a, b, ... -> expr1, expr2, ...
 */
class FunctionParser : public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_FUNCTIONPARSER_H_ */
