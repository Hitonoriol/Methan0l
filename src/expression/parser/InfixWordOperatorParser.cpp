#include <lexer/Token.h>
#include <parser/Parser.h>
#include "expression/parser/InfixWordOperatorParser.h"

#include "expression/BinaryOperatorExpr.h"
#include "expression/Expression.h"
#include "type.h"

namespace mtl
{

/* *[expr1] operator: expr2
 * 		or
 * (expr1) operator: expr2
 */
ExprPtr InfixWordOperatorParser::parse(Parser &parser, ExprPtr lhs, Token token)
{
	parser.consume(TokenType::COLON);
	auto rhs = parser.parse();
	return make_expr<BinaryOperatorExpr>(line(token), lhs, token.get_type(), rhs);
}

} /* namespace mtl */
