#ifndef SRC_EXPRESSION_PARSER_BOXUNITPARSER_H_
#define SRC_EXPRESSION_PARSER_BOXUNITPARSER_H_

#include "UnitParser.h"

#include <expression/UnitExpr.h>

namespace mtl
{

class BoxUnitParser: public UnitParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			Token unit_def = parser.consume(TokenType::BRACE_L);
			ExprPtr unit = UnitParser::parse(parser, unit_def);
			try_cast<UnitExpr>(unit).get_unit_ref().box();
			return unit;
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_BOXUNITPARSER_H_ */
