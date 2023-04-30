#include "ConstParser.h"

#include <parser/Parser.h>

namespace mtl
{

ExprPtr ConstParser::parse(Parser &parser, Token token)
{
	if (!parser.match(TokenType::COLON))
		token.assert_type(TokenType::LIST);

	return parser.evaluate_const(parser.parse(prec));
}

} /* namespace mtl */
