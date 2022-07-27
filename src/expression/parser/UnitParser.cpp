#include "UnitParser.h"

#include <Parser.h>

namespace mtl
{

ExprPtr UnitParser::parse(Parser &parser, Token token)
{
	ExprList exprs;
	if (!parser.match(TokenType::BRACE_R)) {
		do {
			exprs.push_back(parser.parse());
		} while (!parser.match(TokenType::BRACE_R));
	}
	LOG("Parsed a unit with " << exprs.size() << " exprs");

	bool weak = token.get_type() == TokenType::ARROW_R;
	return make_expr<UnitExpr>(line(token), exprs, weak);
}

ExprPtr UnitParser::parse_expr_block(Parser &parser, bool unwrap_single_exprs)
{
	ExprPtr first_expr = parser.parse();

	if (instanceof<UnitExpr>(first_expr))
		return first_expr;

	if (unwrap_single_exprs && !parser.peek(TokenType::COMMA))
		return first_expr;

	ExprList block { first_expr };
	while (parser.match(TokenType::COMMA))
		block.push_back(parser.parse());
	return make_expr<UnitExpr>(block.front()->get_line(), block);
}

ExprPtr UnitParser::parse_ctrl_block(Parser &parser, bool unwrap_single_exprs)
{
	auto block = parse_expr_block(parser, unwrap_single_exprs);
	if_instanceof<UnitExpr>(*block, [](auto &uexpr){
		uexpr.get_unit_ref().expr_block();
	});
	return block;
}

}
