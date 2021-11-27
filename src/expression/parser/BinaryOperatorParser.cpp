#include <expression/Expression.h>
#include <expression/parser/BinaryOperatorParser.h>
#include <Parser.h>
#include <type.h>
#include <Token.h>
#include <memory>

namespace mtl
{

BinaryOperatorParser::BinaryOperatorParser(int precedence, bool is_right) :
		opr_precedence(precedence), right_assoc(is_right)
{
}

ExprPtr BinaryOperatorParser::parse(Parser &parser, ExprPtr lhs, Token token)
{
	bool access_opr = token.get_type() == TokenType::DOT;
	if (access_opr)
		parser.parse_access_opr(true);

	ExprPtr rhs = parser.parse(opr_precedence - (right_assoc ? 1 : 0));

	if (access_opr)
		parser.parse_access_opr(false);
	return make_expr<BinaryOperatorExpr>(line(token), lhs, token.get_type(), rhs);
}

int BinaryOperatorParser::precedence()
{
	return opr_precedence;
}

}
