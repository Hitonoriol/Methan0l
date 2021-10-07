#ifndef SRC_EXPRESSION_PARSER_MAPPARSER_H_
#define SRC_EXPRESSION_PARSER_MAPPARSER_H_

#include "PrefixParser.h"

namespace mtl
{

/* @(key1 => expr1, key2 => expr2, ...) or @() */
class MapParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override;
		static std::string key_string(ExprPtr expr);
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_MAPPARSER_H_ */
