#include <parser/Parser.h>
#include "IndexParser.h"
#include "expression/Expression.h"
#include "expression/PrefixExpr.h"

namespace mtl
{

ExprPtr IndexParser::parse(Parser &parser, ExprPtr lhs, Token token)
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

}
