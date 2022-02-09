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

			/* Stop parsing if a binary non-`=>` token is reached */
			if (!BinaryOperatorExpr::is(*pair_expr, TokenType::KEYVAL)) {
				collector(key_string(parser.parse(0, true)), LiteralExpr::empty());
				break;
			} else
				parser.consume_peeked(peeked.descriptor);

			/* Collect key-value pair: `key => value_expr` */
			BinaryOperatorExpr &pair = try_cast<BinaryOperatorExpr>(pair_expr);
			collector(key_string(pair.get_lhs()), pair.get_rhs());
		} while (parser.match(TokenType::COMMA));

		if (ends_with != TokenType::NONE)
			parser.consume(ends_with);
	}
}

std::string MapParser::key_string(ExprPtr expr)
{
	if (instanceof<IdentifierExpr>(expr.get()))
		return IdentifierExpr::get_name(expr);

	if (instanceof<LiteralExpr>(expr.get()))
		return try_cast<LiteralExpr>(expr).raw_value().to_string();

	throw std::runtime_error("Invalid map key expression: " + expr->info());
}

}
