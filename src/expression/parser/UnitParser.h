#ifndef SRC_EXPRESSION_PARSER_UNITPARSER_H_
#define SRC_EXPRESSION_PARSER_UNITPARSER_H_

#include "../../methan0l_type.h"
#include "../UnitExpr.h"
#include "PrefixParser.h"

namespace mtl
{

class UnitParser: public PrefixParser
{
	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			ExprList exprs;

			if (!parser.match(TokenType::BRACE_R)) {
				do {
					exprs.push_back(parser.parse());
				} while (!parser.match(TokenType::BRACE_R));
			}

			return ptr(new UnitExpr(exprs));
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_UNITPARSER_H_ */
