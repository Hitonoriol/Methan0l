#ifndef EXPRESSION_PARSER_IFELSEPARSER_H_
#define EXPRESSION_PARSER_IFELSEPARSER_H_

#include "PrefixParser.h"

namespace mtl
{

class IfElseParser : public PrefixParser {
	public:
		using PrefixParser::PrefixParser;
		ExprPtr parse(Parser &parser, Token token) override;
};

}

#endif // EXPRESSION_PARSER_IFELSEPARSER_H_