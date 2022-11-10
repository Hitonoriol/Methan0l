#include <lexer/Token.h>
#include <parser/Parser.h>
#include "ClassParser.h"

#include <memory>
#include <string>

#include "type.h"
#include "../ClassExpr.h"
#include "MapParser.h"

namespace mtl
{

/*
 * class: ClassName [base: Foo, Bar, ...] [=] @( ... )
 */
ExprPtr ClassParser::parse(Parser &parser, mtl::Token token)
{
	parser.consume(TokenType::COLON);
	std::string name = parser.consume(TokenType::IDENTIFIER).get_value();

	std::vector<std::string> base;
	if (parser.match(TokenType::BASE_CLASS)) {
		parser.consume(TokenType::COLON);
		do {
			base.push_back(parser.consume(TokenType::IDENTIFIER).get_value());
		} while (parser.match(TokenType::COMMA));
	}

	parser.match(TokenType::ASSIGN);

	auto end_token = TokenType::PAREN_R;
	if (!parser.match(TokenType::MAP_DEF_L)) {
		parser.consume(TokenType::BRACE_L);
		end_token = TokenType::BRACE_R;
	}

	ExprMap body_map;
	MapParser::parse(parser, [&body_map](auto name,
			auto expr) {body_map.emplace(name, expr);}, end_token);
	return make_expr<ClassExpr>(line(token), name, base, body_map);
}

} /* namespace mtl */
