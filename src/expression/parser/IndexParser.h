#ifndef SRC_EXPRESSION_PARSER_INDEXPARSER_H_
#define SRC_EXPRESSION_PARSER_INDEXPARSER_H_

#include "InfixParser.h"
#include "../IndexExpr.h"
#include "../IdentifierExpr.h"

namespace mtl
{

/*
 * Append a new element: list[] = expr
 * Index element: list[expr]
 * Remove element: list[~expr]
 */
class IndexParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token)
		{
			ExprPtr idx = nullptr;
			bool remove = false;

			if (!parser.match(TokenType::BRACKET_R)) {
				remove = parser.match(TokenType::TILDE);
				idx = parser.parse();
				parser.consume(TokenType::BRACKET_R);
			}

			ExprPtr expr = make_expr<IndexExpr>(line(token), lhs, idx, remove);

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
