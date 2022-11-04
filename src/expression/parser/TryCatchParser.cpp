#include "TryCatchParser.h"

#include <expression/parser/UnitParser.h>
#include <expression/Expression.h>
#include <expression/TryCatchExpr.h>
#include <expression/UnitExpr.h>
#include <lexer/Token.h>
#include <parser/Parser.h>
#include <type.h>
#include <algorithm>
#include <string>

namespace mtl
{

/*
 * try {...} catch: ex_name {...}
 */
ExprPtr TryCatchParser::parse(Parser &parser, Token token)
{
	ExprPtr try_body = UnitParser::parse_ctrl_block(parser, false);
	parser.consume(TokenType::CATCH);
	parser.consume(TokenType::COLON);
	auto as = parser.parse();
	std::string catch_as = std::move(Expression::get_name(as));
	ExprPtr catch_body = UnitParser::parse_ctrl_block(parser, false);
	return make_expr<TryCatchExpr>(line(token), try_body, catch_body, std::move(catch_as));
}

} /* namespace mtl */
