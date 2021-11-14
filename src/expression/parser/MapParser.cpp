#include "MapParser.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "../../Parser.h"
#include "../../structure/Value.h"
#include "../../type.h"
#include "../../Token.h"
#include "../../util/util.h"
#include "../BinaryOperatorExpr.h"
#include "../IdentifierExpr.h"
#include "../LiteralExpr.h"
#include "../MapExpr.h"

namespace mtl
{

ExprPtr MapParser::parse(Parser &parser, Token token)
{
	ExprMap map;

	if constexpr (DEBUG)
		std::cout << "Parsing map expr..." << std::endl;

	parse(parser, [&](std::string key, ExprPtr val) {map.emplace(key, val);});

	if constexpr (DEBUG)
		std::cout << "Parsed a map with " << map.size() << " entries" << std::endl;

	return make_expr<MapExpr>(line(token), map);
}

/* Assumes that the `@(` token is already consumed */
void MapParser::parse(Parser &parser,
		std::function<void(std::string, ExprPtr)> collector)
{
	if (!parser.match(TokenType::PAREN_R)) {
		do {
			ExprPtr pair_expr = parser.parse();

			/* Key with no value specified */
			if (instanceof<IdentifierExpr>(pair_expr.get())
					|| instanceof<LiteralExpr>(pair_expr.get())) {
				collector(key_string(pair_expr), LiteralExpr::empty());
				continue;
			}

			BinaryOperatorExpr &pair = try_cast<BinaryOperatorExpr>(pair_expr);
			if (pair.get_operator() != TokenType::KEYVAL)
				throw std::runtime_error("Unexpected opr in map declaration");

			collector(key_string(pair.get_lhs()), pair.get_rhs());
		} while (parser.match(TokenType::COMMA));
		parser.consume(TokenType::PAREN_R);
	}
}

std::string MapParser::key_string(ExprPtr expr)
{
	if (instanceof<IdentifierExpr>(expr.get()))
		return IdentifierExpr::get_name(expr);

	if (instanceof<LiteralExpr>(expr.get()))
		return Value(try_cast<LiteralExpr>(expr).raw_value()).to_string();

	throw std::runtime_error("Unsupported map key type");
}

}
