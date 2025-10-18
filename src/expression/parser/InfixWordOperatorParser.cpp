#include "InfixWordOperatorParser.h"

#include <lexer/Token.h>
#include <parser/Parser.h>
#include <parser/expression/Expression.h>
#include <expression/BinaryOperatorExpr.h>

namespace mtl
{

/* *[expr1] operator: expr2
 * 		or
 * (expr1) operator: expr2
 */
ExprPtr InfixWordOperatorParser::parse(Parser &parser, ExprPtr lhs, Token token)
{
	parser.consume(Tokens::COLON);
	auto rhs = parser.parse();
	return make_expr<BinaryOperatorExpr>(line(token), lhs, token.get_type(), rhs);
}

} /* namespace mtl */
