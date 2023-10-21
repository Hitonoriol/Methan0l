#include "TryCatchParser.h"

#include <algorithm>
#include <string>

#include <lexer/Token.h>
#include <parser/Parser.h>
#include <expression/parser/UnitParser.h>
#include <expression/Expression.h>
#include <expression/TryCatchExpr.h>
#include <expression/UnitExpr.h>

namespace mtl
{

/*
 * try {...} catch: ex_name {...}
 */
ExprPtr TryCatchParser::parse(Parser &parser, Token token)
{
	ExprPtr try_body = UnitParser::parse_ctrl_block(parser, false);
	parser.consume(Tokens::CATCH);
	parser.consume(Tokens::COLON);
	auto as = parser.parse();
	std::string catch_as = Expression::get_name(as);
	ExprPtr catch_body = UnitParser::parse_ctrl_block(parser, false);
	return make_expr<TryCatchExpr>(line(token), try_body, catch_body, std::move(catch_as));
}

} /* namespace mtl */
