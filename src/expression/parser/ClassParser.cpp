#include "ClassParser.h"

#include <memory>
#include <string>

#include <lexer/Token.h>
#include <parser/Parser.h>
#include <expression/ClassExpr.h>
#include <expression/parser/MapParser.h>

namespace mtl
{

/*
 * class: ClassName [base: Foo] [implements: Bar, Baz, ...] { ... }
 */
ExprPtr ClassParser::parse(Parser &parser, mtl::Token token)
{
	parser.consume(Tokens::COLON);
	std::string name = parser.consume(Tokens::IDENTIFIER).get_value();

	std::string base;
	std::vector<std::string> interfaces;
	if (parser.match(Tokens::BASE_CLASS)) {
		parser.consume(Tokens::COLON);
		base = parser.consume(Tokens::IDENTIFIER).get_value();
	}

	if (parser.match(Tokens::IMPLEMENT)) {
		parser.consume(Tokens::COLON);
		do {
			interfaces.push_back(parser.consume(Tokens::IDENTIFIER).get_value());
		} while (parser.match(Tokens::COMMA));
	}

	auto end_token = Tokens::PAREN_R;
	if (!parser.match(Tokens::MAP_DEF_L)) {
		parser.consume(Tokens::BRACE_L);
		end_token = Tokens::BRACE_R;
	}

	ExprMap body_map;
	MapParser::parse(parser, [&body_map](auto name_expr, auto expr) {
		body_map.emplace(MapParser::key_string(name_expr), expr);
	}, end_token);

	auto class_expr = make_expr<ClassExpr>(line(token), name, body_map);
	class_expr->set_base(base);
	class_expr->set_interfaces(std::move(interfaces));
	return class_expr;
}

} /* namespace mtl */
