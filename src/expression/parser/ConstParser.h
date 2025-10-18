#ifndef SRC_EXPRESSION_PARSER_CONSTPARSER_H_
#define SRC_EXPRESSION_PARSER_CONSTPARSER_H_

#include <parser/expression/PrefixExprParser.h>

namespace mtl
{

class ConstParser: public PrefixParser
{
	public:
		using PrefixParser::PrefixParser;

		ExprPtr parse(Parser &parser, Token token) override;

		ExprPtr retrieve_const_value();
		ExprPtr evaluate_const();
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_CONSTPARSER_H_ */
