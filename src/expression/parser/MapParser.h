#ifndef SRC_EXPRESSION_PARSER_MAPPARSER_H_
#define SRC_EXPRESSION_PARSER_MAPPARSER_H_

#include <lexer/Token.h>
#include "PrefixParser.h"

namespace mtl
{

/* @(key1 => expr1, key2 => expr2, ...) or @() */
class MapParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override;
		static void parse(Parser &parser, std::function<void(std::string, ExprPtr)> collector, TokenType ends_with = TokenType::PAREN_R);
		static std::string key_string(ExprPtr expr);
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_MAPPARSER_H_ */
