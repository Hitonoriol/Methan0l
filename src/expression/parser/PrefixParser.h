#ifndef EXPRESSION_PARSER_PREFIXPARSER_H_
#define EXPRESSION_PARSER_PREFIXPARSER_H_

#include "../../type.h"

namespace mtl
{

class Parser;

class PrefixParser
{
	public:
		virtual ExprPtr parse(Parser &parser, Token token) = 0;
		virtual ~PrefixParser() = default;
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_PREFIXPARSER_H_ */
