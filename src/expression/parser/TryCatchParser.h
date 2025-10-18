#ifndef SRC_EXPRESSION_PARSER_TRYCATCHPARSER_H_
#define SRC_EXPRESSION_PARSER_TRYCATCHPARSER_H_

#include <parser/expression/PrefixExprParser.h>

namespace mtl
{

class TryCatchParser: public PrefixParser
{
	public:
		virtual ExprPtr parse(Parser &parser, Token token) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_TRYCATCHPARSER_H_ */
