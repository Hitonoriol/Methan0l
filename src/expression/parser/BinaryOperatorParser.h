#ifndef SRC_EXPRESSION_PARSER_BINARYOPERATORPARSER_H_
#define SRC_EXPRESSION_PARSER_BINARYOPERATORPARSER_H_

#include <parser/expression/InfixParser.h>
#include <expression/BinaryOperatorExpr.h>

namespace mtl
{

class BinaryOperatorParser: public InfixParser
{
	private:
		int opr_precedence;
		bool right_assoc;

	public:
		BinaryOperatorParser(int precedence, bool is_right);
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override;
		int precedence() override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_BINARYOPERATORPARSER_H_ */
