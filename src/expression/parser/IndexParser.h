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
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token)
		{
			ExprPtr idx = nullptr;
			bool remove = false;
			bool insert = false;

			if (!parser.match(TokenType::BRACKET_R)) {
				remove = parser.match(TokenType::TILDE);
				/* If this expr is not `expr[~]` */
				if (!parser.match(TokenType::BRACKET_R)) {
					insert = parser.match(TokenType::ARROW_R);

					bool foreach_expr;
					if ((foreach_expr = parser.match(TokenType::DO)))
						parser.consume(TokenType::COLON);

					idx = foreach_expr ?
							make_expr<PrefixExpr>(line(token), TokenType::DO, parser.parse()) :
							parser.parse();
					parser.consume(TokenType::BRACKET_R);
				}
			}

			ExprPtr expr = make_expr<IndexExpr>(line(token), lhs, idx, remove, insert);

			if (parser.match(TokenType::BRACKET_L))
				expr = parse(parser, expr, token);

			return expr;
		}

		int precedence() override
		{
			return static_cast<int>(Precedence::INDEX);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_INDEXPARSER_H_ */
