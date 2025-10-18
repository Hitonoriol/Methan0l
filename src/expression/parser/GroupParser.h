#ifndef EXPRESSION_PARSER_GROUPPARSER_H_
#define EXPRESSION_PARSER_GROUPPARSER_H_

#include <parser/expression/PrefixExprParser.h>

namespace mtl
{

/*
 * (expr)
 * *[expr]
 */
class GroupParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token);
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_GROUPPARSER_H_ */
