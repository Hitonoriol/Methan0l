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
 */
ExprPtr ClassParser::parse(Parser &parser, mtl::Token token)
{
	parser.consume(TokenType::COLON);
	std::string name = parser.consume(TokenType::IDENTIFIER).get_value();
	parser.consume(TokenType::ASSIGN);
	ExprPtr body = parser.parse();

	if (!instanceof<MapExpr>(body.get()))
		throw std::runtime_error("Class definition body must be a Map Expression");

	return std::make_shared<ClassExpr>(name, body);
}

} /* namespace mtl */
