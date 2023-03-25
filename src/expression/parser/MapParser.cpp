#include <lexer/Token.h>
#include <parser/Parser.h>
#include "MapParser.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "structure/Value.h"
#include "type.h"
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
	ExprExprMap map;
	auto start_tok = token.get_type();
	auto end_tok = TokenType::PAREN_R;

	/* @[...] */
	if (start_tok != TokenType::MAP_DEF_L) {
		token.assert_type(TokenType::MAP_DEF_L_ALT);
		end_tok = TokenType::BRACKET_R;
	}

	parse(parser, [&](auto key, auto val) {
		map.emplace(key, val);
	}, end_tok);
	return make_expr<MapExpr>(line(token), map);
}

/* Assumes that the `@(` token is already consumed */
void MapParser::parse(Parser &parser,
		ExprPairCollector collector,
		TokenType ends_with)
{
	if (parser.match(ends_with))
		return;

	bool has_end_tok = ends_with != TokenType::NONE;
	do {
		auto key_expr = parser.parse();
		/* If there's no `=>`, this is just a key with no value specified */
		if (!parser.match(TokenType::KEYVAL)) {
			/* Capture expression: `%var_name`
			 * (behaves the same as: var_name => **var_name) */
			if (PrefixExpr::is(*key_expr, TokenType::PERCENT)) {
				auto idfr = try_cast<PrefixExpr>(key_expr).get_rhs();
				collector(idfr, make_expr<PrefixExpr>(0, Token(TokenType::REF), idfr));
				continue;
			}
			else
				collector(key_expr, LiteralExpr::empty());
			continue;
		}

		/* Otherwise, this is a key-value pair: `key_expr => value_expr` */
		auto value_expr = parser.parse();
		collector(key_expr, value_expr);

		/* Look for key-value pair delimiters: `,` or newline / `;` */
	} while (parser.match(TokenType::COMMA)
			|| (has_end_tok && parser.look_ahead() != ends_with));

	if (has_end_tok)
		parser.consume(ends_with);
}

std::string MapParser::key_string(Expression &expr)
{
	if (instanceof<IdentifierExpr>(expr))
		return IdentifierExpr::get_name(&expr);

	if (instanceof<LiteralExpr>(expr))
		return try_cast<LiteralExpr>(&expr).raw_ref().to_string();

	throw std::runtime_error("Invalid map key expression: " + expr.info());
}

}
