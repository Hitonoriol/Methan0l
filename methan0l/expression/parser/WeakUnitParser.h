#ifndef SRC_EXPRESSION_PARSER_WEAKUNITPARSER_H_
#define SRC_EXPRESSION_PARSER_WEAKUNITPARSER_H_

#include "UnitParser.h"

namespace mtl
{

class WeakUnitParser: public UnitParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_WEAKUNITPARSER_H_ */
