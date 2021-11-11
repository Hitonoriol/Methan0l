#ifndef SRC_EXPRESSION_PARSER_WORDOPERATORPARSER_H_
#define SRC_EXPRESSION_PARSER_WORDOPERATORPARSER_H_

#include "PrefixOperatorParser.h"

namespace mtl
{

class WordOperatorParser: public PrefixOperatorParser
{
	public:
		WordOperatorParser() = default;
		WordOperatorParser(int prec);
		ExprPtr parse(Parser &parser, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_WORDOPERATORPARSER_H_ */
