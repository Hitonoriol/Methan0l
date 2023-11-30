#include "UnitParser.h"

#include <type.h>
#include <parser/Parser.h>
#include <expression/UnitExpr.h>
#include <interpreter/Interpreter.h>

namespace mtl
{

ExprPtr UnitParser::parse(Parser &parser, Token token)
{
	ExprList exprs(parser.get_context().allocator<ExprPtr>());
	if (!parser.match(Tokens::BRACE_R)) {
		do {
			exprs.push_back(parser.parse());
		} while (!parser.match(Tokens::BRACE_R));
	}
	LOG("Parsed a unit with " << exprs.size() << " exprs");

	bool weak = token.get_type() == Tokens::ARROW_R;
	return make_expr<UnitExpr>(line(token), &parser.get_context(), exprs, weak);
}

ExprPtr UnitParser::parse_expr_block(Parser &parser, bool unwrap_single_exprs)
{
	ExprPtr first_expr = parser.parse();

	if (instanceof<UnitExpr>(first_expr))
		return first_expr;

	if (unwrap_single_exprs && !parser.peek(Tokens::COMMA))
		return first_expr;

	ExprList block(parser.get_context().allocator<ExprPtr>());
	block.push_back(first_expr);

	while (parser.match(Tokens::COMMA))
		block.push_back(parser.parse());
	return make_expr<UnitExpr>(block.front()->get_line(), &parser.get_context(), block);
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
