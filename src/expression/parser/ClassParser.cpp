#include "ClassParser.h"

#include <memory>
#include <string>

#include "../../Parser.h"
#include "../../type.h"
#include "../../Token.h"
#include "../ClassExpr.h"

namespace mtl
{

/*
 * class: ClassName = @(private => $(), ...)
 * 		or
 * class: ClassName @(private => $(), ...)
 */
ExprPtr ClassParser::parse(Parser &parser, mtl::Token token)
{
	parser.consume(TokenType::COLON);
	std::string name = parser.consume(TokenType::IDENTIFIER).get_value();
	parser.match(TokenType::ASSIGN);
	ExprPtr body = parser.parse();
	body->assert_type<MapExpr>("Class definition body must be a Map Expression");
	return make_expr<ClassExpr>(line(token),name, body);
}

} /* namespace mtl */
