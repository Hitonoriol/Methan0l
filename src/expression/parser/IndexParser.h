#ifndef SRC_EXPRESSION_PARSER_INDEXPARSER_H_
#define SRC_EXPRESSION_PARSER_INDEXPARSER_H_

#include "InfixParser.h"
#include "../IndexExpr.h"
#include "../IdentifierExpr.h"

namespace mtl
{

/* All types:
 * 	Remove element: ctr[~expr]
 * 	Clear container: ctr[~]
 * 	For-each: ctr[do: funcexpr]
 *
 * Lists & Strings:
 * 	Append a new element: list[] = expr
 * 	Index element: list[expr]
 *
 * Maps:
 * 	Add a new element: map[expr1] = expr2
 * 	Access an element: map[expr]
 *
 * Sets:
 * 	Insert a new element: set[->expr]
 * 	Test element's existence: set[expr]
 */
class IndexParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token);
		virtual ~IndexParser() = default;

		int precedence() override
		{
			return static_cast<int>(Precedence::INDEX);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_INDEXPARSER_H_ */
