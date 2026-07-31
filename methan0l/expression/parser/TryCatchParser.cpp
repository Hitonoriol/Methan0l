#include "TryCatchParser.h"

#include <algorithm>
#include <string>

#include <ch3oh/lexer/Token.h>
#include <ch3oh/parser/Parser.h>
#include <methan0l/expression/parser/UnitParser.h>
#include <ch3oh/parser/expression/Expression.h>
#include <methan0l/expression/TryCatchExpr.h>
#include <methan0l/expression/UnitExpr.h>
#include <methan0l/core/ExpressionUtils.h>

namespace mtl
{

/*
 * <try> <:> {...} <catch> <:> <ex_name> {...}
 */
ExprPtr TryCatchParser::parse(Parser &parser, Token token)
{
	parser.consume(Tokens::COLON);
	ExprPtr try_body = UnitParser::parse_ctrl_block(parser, false);
	parser.consume(Tokens::CATCH);
	parser.consume(Tokens::COLON);
	auto as = parser.parse();
	std::string catch_as = ExpressionUtils::get_name(as);
	ExprPtr catch_body = UnitParser::parse_ctrl_block(parser, false);
	return make_expr<TryCatchExpr>(line(token), try_body, catch_body, std::move(catch_as));
}

} /* namespace mtl */
