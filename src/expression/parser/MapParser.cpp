#include "MapParser.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "Parser.h"
#include "structure/Value.h"
#include "type.h"
#include "Token.h"
#include "util/util.h"

#include "../BinaryOperatorExpr.h"
#include "../IdentifierExpr.h"
#include "../LiteralExpr.h"
#include "../PrefixExpr.h"
#include "../MapExpr.h"

namespace mtl
{

ExprPtr MapParser::parse(Parser &parser, Token token)
{
	ExprMap map;
	parse(parser, [&](std::string key, ExprPtr val) {map.emplace(key, val);});
	return make_expr<MapExpr>(line(token), map);
}

/* Assumes that the `@(` token is already consumed */
void MapParser::parse(Parser &parser,
		std::function<void(std::string, ExprPtr)> collector,
		TokenType ends_with)
{
	bool has_end_tok = ends_with != TokenType::NONE;
	if (!parser.match(ends_with)) {
		do {
			auto peeked = parser.peek_parse();
			ExprPtr pair_expr = peeked.expression;
			/* Collect key with no value specified */
			if (instanceof<IdentifierExpr>(pair_expr)
					|| instanceof<LiteralExpr>(pair_expr)) {
				parser.consume_peeked(peeked.descriptor);
				collector(key_string(pair_expr), LiteralExpr::empty());
				continue;
			}

			/* Capture expression: `%var_name`
			 * (behaves the same as: var_name => **var_name) */
			if (PrefixExpr::is(*pair_expr, TokenType::PERCENT)) {
				auto idfr = try_cast<PrefixExpr>(pair_expr).get_rhs();
				collector(key_string(idfr), make_expr<PrefixExpr>(0, Token(TokenType::REF), idfr));
				parser.consume_peeked(peeked.descriptor);
				continue;
			}

			/* Stop parsing if a non-`=>` binary operator is reached */
			if (!BinaryOperatorExpr::is(*pair_expr, TokenType::KEYVAL)) {
				auto &binex = try_cast<BinaryOperatorExpr>(pair_expr);
				/* If LHS of this invalid expression is a valid key-value pair, consume it partially */
				if (BinaryOperatorExpr::is(*binex.get_lhs(), TokenType::KEYVAL)) {
					parser.consume_peeked(peeked.descriptor, binex.get_token());
					pair_expr = binex.get_lhs();
				}
				else {
					/* Otherwise, don't consume peeked expr and reparse it as a prefix expression */
					collector(key_string(parser.parse(0, true)), LiteralExpr::empty());
					break;
				}
			}
			else
				parser.consume_peeked(peeked.descriptor);


			/* Collect key-value pair: `key => value_expr` */
			BinaryOperatorExpr &pair = try_cast<BinaryOperatorExpr>(pair_expr);
			collector(key_string(pair.get_lhs()), pair.get_rhs());

			/* Key-value pair delimiters: `,` or newline / `;` */
		} while (parser.match(TokenType::COMMA) || (has_end_tok && parser.look_ahead() != ends_with));

		if (has_end_tok)
			parser.consume(ends_with);
	}
}

std::string MapParser::key_string(ExprPtr expr)
{
	if (instanceof<IdentifierExpr>(expr.get()))
		return IdentifierExpr::get_name(expr);

	if (instanceof<LiteralExpr>(expr.get()))
		return try_cast<LiteralExpr>(expr).raw_ref().to_string();

	throw std::runtime_error("Invalid map key expression: " + expr->info());
}

}
