#ifndef SRC_EXPRESSION_PARSER_CLASSPARSER_H_
#define SRC_EXPRESSION_PARSER_CLASSPARSER_H_

#include "PrefixParser.h"

namespace mtl
{

class ClassParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_CLASSPARSER_H_ */
