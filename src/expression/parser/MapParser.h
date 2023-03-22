#ifndef SRC_EXPRESSION_PARSER_MAPPARSER_H_
#define SRC_EXPRESSION_PARSER_MAPPARSER_H_

#include "PrefixParser.h"

#include <lexer/Token.h>

namespace mtl
{

/* 1. @(key_expr1 => val_expr1, key_expr2 => val_expr2, ...) or @()
 * 2. @[key_expr1 => val_expr1, key_expr2 => val_expr2, ...] or @[] */
class MapParser : public PrefixParser
{
	public:
		using ExprPairCollector = std::function<void(ExprPtr, ExprPtr)>;

		ExprPtr parse(Parser &parser, Token token) override;

		static void parse(Parser &parser,
				ExprPairCollector collector,
				TokenType ends_with = TokenType::PAREN_R);

		static std::string key_string(ExprPtr expr);
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_MAPPARSER_H_ */
