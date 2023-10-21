#include "IndexParser.h"

#include <parser/Parser.h>
#include <expression/Expression.h>
#include <expression/PrefixExpr.h>

namespace mtl
{

ExprPtr IndexParser::parse(Parser &parser, ExprPtr lhs, Token token)
{
	ExprPtr idx = nullptr;
	bool remove = false;
	bool insert = false;

	if (!parser.match(Tokens::BRACKET_R)) {
		remove = parser.match(Tokens::TILDE);
		/* If this expr is not `expr[~]` */
		if (!parser.match(Tokens::BRACKET_R)) {
			insert = parser.match(Tokens::ARROW_R);

			bool foreach_expr;
			if ((foreach_expr = parser.match(Tokens::DO)))
				parser.consume(Tokens::COLON);

			idx = foreach_expr ?
					make_expr<PrefixExpr>(line(token), Tokens::DO, parser.parse()) :
					parser.parse();
			parser.consume(Tokens::BRACKET_R);
		}
	}

	ExprPtr expr = make_expr<IndexExpr>(line(token), lhs, idx, remove, insert);

	if (parser.match(Tokens::BRACKET_L))
		expr = parse(parser, expr, token);

	return expr;
}

}
