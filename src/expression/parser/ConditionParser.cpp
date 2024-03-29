#include "ConditionParser.h"

#include <expression/parser/UnitParser.h>
#include <parser/Parser.h>
#include <expression/ConditionalExpr.h>
#include <expression/LiteralExpr.h>
#include <expression/UnitExpr.h>

namespace mtl
{

ExprPtr ConditionParser::parse(Parser &parser, ExprPtr lhs, Token token)
{
	ExprPtr then_expr = UnitParser::parse_ctrl_block(parser);
	ExprPtr else_expr =
			parser.match(Tokens::COLON) ?
					UnitParser::parse_ctrl_block(parser) :
					make_expr<LiteralExpr>(line(token));

	return make_expr<ConditionalExpr>(line(token), lhs, then_expr, else_expr);
}

}
