#ifndef SRC_EXPRESSION_PARSER_RANGEPARSER_H_
#define SRC_EXPRESSION_PARSER_RANGEPARSER_H_

#include <parser/expression/InfixParser.h>

namespace mtl
{

/* Range expression:
 * 	(1)		start..end
 * 	(2)		start..end..step
 */
class RangeParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override;

		inline int precedence() override
		{
			return prcdc(Precedence::STRING_CONCAT);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_RANGEPARSER_H_ */
