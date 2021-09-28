#ifndef EXPRESSION_PARSER_PREFIXPARSER_H_
#define EXPRESSION_PARSER_PREFIXPARSER_H_

#include <memory>

#include "../../Parser.h"
#include "../../Token.h"
#include "../Expression.h"
#include "../../methan0l_type.h"

namespace mtl
{

class PrefixParser
{
	public:
		virtual ExprPtr parse(Parser &parser, Token token) = 0;
		virtual ~PrefixParser() = default;
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_PREFIXPARSER_H_ */
