#include "IfElseParser.h"

#include <lexer/Token.h>
#include <parser/Parser.h>

#include <expression/parser/UnitParser.h>
#include <expression/ConditionalExpr.h>

namespace mtl
{

/*
 * if: <expression> <expression> [else:] <expression>
 */
ExprPtr IfElseParser::parse(Parser &parser, Token token)
{
	parser.match(Tokens::COLON);
	auto condition = parser.parse();
	auto then_branch = UnitParser::parse_ctrl_block(parser);

	ExprPtr else_branch;
	if (parser.match(Tokens::ELSE)) {
		parser.consume(Tokens::COLON);
		else_branch = UnitParser::parse_ctrl_block(parser);
	} else {
		else_branch = make_expr<LiteralExpr>(line(token));
	}

	return make_expr<ConditionalExpr>(line(token), condition, then_branch, else_branch);
}

}