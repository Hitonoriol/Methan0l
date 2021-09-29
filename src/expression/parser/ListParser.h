#ifndef SRC_EXPRESSION_PARSER_LISTPARSER_H_
#define SRC_EXPRESSION_PARSER_LISTPARSER_H_

#include "PrefixParser.h"
#include "../ListExpr.h"

namespace mtl
{

class ListParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			ExprList raw_list;
			if (!parser.match(TokenType::PAREN_R)) {
				do {
					raw_list.push_back(parser.parse());
				} while (parser.match(TokenType::COMMA));
				parser.consume(TokenType::PAREN_R);
			}
			return ptr(new ListExpr(raw_list));
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_LISTPARSER_H_ */
