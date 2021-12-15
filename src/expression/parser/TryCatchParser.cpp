#include "TryCatchParser.h"

#include <expression/Expression.h>
#include <expression/TryCatchExpr.h>
#include <expression/UnitExpr.h>
#include <Parser.h>
#include <type.h>
#include <Token.h>
#include <algorithm>
#include <string>

namespace mtl
{

/*
 * try {...} catch: ex_name {...}
 */
ExprPtr TryCatchParser::parse(Parser &parser, Token token)
{
	ExprPtr try_body = parser.parse();
	parser.consume(TokenType::CATCH);
	parser.consume(TokenType::COLON);
	auto as = parser.parse();
	std::string catch_as = std::move(Expression::get_name(as));
	ExprPtr catch_body = parser.parse();
	catch_body->assert_type<UnitExpr>("Catch expression body must be a Unit");

	return make_expr<TryCatchExpr>(line(token), try_body, catch_body, std::move(catch_as));
}

} /* namespace mtl */
