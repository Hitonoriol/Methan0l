#include "GroupParser.h"

#include <memory>

#include <ch3oh/lexer/Token.h>
#include <ch3oh/parser/Parser.h>
#include <methan0l/expression/LiteralExpr.h>
#include <methan0l/expression/TypeRefExpr.h>
#include <methan0l/util/cast.h>

namespace mtl
{

ExprPtr GroupParser::parse(Parser &parser, Token token)
{
	/* Alternative group syntax: *[exprs] */
	bool alt_group = token.get_type() == Tokens::INFIX_WORD_LHS_L;
	/* Empty group */
	if (parser.match(Tokens::PAREN_R)
			|| (alt_group && parser.match(Tokens::BRACKET_R)))
		return std::make_shared<LiteralExpr>();

	/* Type reference expression: (TypeName) */
	else if (parser.peek(Tokens::IDENTIFIER) && parser.peek(Tokens::PAREN_R, 1)) {
		auto type_tok = parser.consume(Tokens::IDENTIFIER);
		parser.consume(Tokens::PAREN_R);
		return make_expr<TypeRefExpr>(line(token), type_tok.get_value());
	}

	auto expr = parser.parse();
	parser.consume(!alt_group ? Tokens::PAREN_R : Tokens::BRACKET_R);
	return expr;
}

}
