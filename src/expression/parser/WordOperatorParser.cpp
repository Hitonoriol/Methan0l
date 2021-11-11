#include "WordOperatorParser.h"

#include <memory>

#include "../../Parser.h"
#include "../../type.h"
#include "../../Token.h"
#include "../Expression.h"
#include "../PrefixExpr.h"

namespace mtl
{

WordOperatorParser::WordOperatorParser(int prec) : PrefixOperatorParser(prec)
{
}

/*
 * operator: expr
 * operator(expr)
 */
ExprPtr WordOperatorParser::parse(Parser &parser, Token token)
{
	parser.match(TokenType::COLON);
	ExprPtr rhs = parser.parse();

	if constexpr (DEBUG)
		out << "Word expr // " << token.to_string() << ": " << rhs->info() << std::endl;

	return make_expr<PrefixExpr>(line(token), token.get_type(), rhs);
}

} /* namespace mtl */
