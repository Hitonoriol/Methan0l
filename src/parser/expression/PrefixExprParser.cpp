#include "PrefixExprParser.h"
#include "parser/Parser.h"

namespace mtl
{

ExprPtr PrefixParser::parse(Parser& parser, Token token)
{
	ExprPtr rhs = parser.parse(prec);
	return make_expr<PrefixExpr>(line(token), token, rhs);
}

}