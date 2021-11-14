#ifndef SRC_EXPRESSION_PARSER_LISTPARSER_H_
#define SRC_EXPRESSION_PARSER_LISTPARSER_H_

#include "PrefixParser.h"
#include "../ListExpr.h"

namespace mtl
{

class ListParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override;
		static void parse(Parser &parser, std::function<void(ExprPtr)> collector);
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_LISTPARSER_H_ */
