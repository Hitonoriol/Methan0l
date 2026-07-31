#ifndef SRC_EXPRESSION_PARSER_INFIXWORDOPERATORPARSER_H_
#define SRC_EXPRESSION_PARSER_INFIXWORDOPERATORPARSER_H_

#include "BinaryOperatorParser.h"

namespace mtl
{

class InfixWordOperatorParser: public BinaryOperatorParser
{
	public:
		InfixWordOperatorParser(int precedence, bool is_right) : BinaryOperatorParser(precedence, is_right) {}
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_INFIXWORDOPERATORPARSER_H_ */
