#ifndef SRC_EXPRESSION_PARSER_MAPPARSER_H_
#define SRC_EXPRESSION_PARSER_MAPPARSER_H_

#include "PrefixParser.h"

#include "../LiteralExpr.h"
#include "../MapExpr.h"
#include "../BinaryOperatorExpression.h"

namespace mtl
{

/* @(key1 : expr1, key2 : expr2, ...) or @() */
class MapParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			ExprMap map;

			if constexpr (DEBUG)
				std::cout << "Parsing map expr..." << std::endl;

			if (!parser.match(TokenType::PAREN_R)) {
				do {
					ExprPtr pair_expr = parser.parse();

					/* Key with no value specified */
					if (instanceof<IdentifierExpr>(pair_expr.get())) {
						map.emplace(IdentifierExpr::get_name(pair_expr), ptr(new LiteralExpr()));
						continue;
					}

					BinaryOperatorExpression &pair = try_cast<BinaryOperatorExpression>(pair_expr);
					if (pair.get_operator() != TokenType::KEYVAL)
						throw std::runtime_error("Unexpected opr in map declaration");

					if (!instanceof<IdentifierExpr>(pair.get_lhs().get()))
						throw std::runtime_error("Map entry LHS must be an Identifier");

					map.emplace(IdentifierExpr::get_name(pair.get_lhs()), pair.get_rhs());

				} while (parser.match(TokenType::COMMA));
				parser.consume(TokenType::PAREN_R);
			}

			if constexpr (DEBUG)
				std::cout << "Parsed a map with " << map.size() << " entries" << std::endl;

			return ptr(new MapExpr(map));
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_MAPPARSER_H_ */
