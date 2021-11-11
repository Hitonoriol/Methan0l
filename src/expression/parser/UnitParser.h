#ifndef SRC_EXPRESSION_PARSER_UNITPARSER_H_
#define SRC_EXPRESSION_PARSER_UNITPARSER_H_

#include "../../type.h"
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

			if constexpr (DEBUG)
				std::cout << "Parsing unit expr..." << std::endl;

			if (!parser.match(TokenType::BRACE_R)) {
				do {
					exprs.push_back(parser.parse());
				} while (!parser.match(TokenType::BRACE_R));
			}

			bool weak = token.get_type() == TokenType::ARROW_R;

			if constexpr (DEBUG)
				std::cout << "Parsed a unit with " << exprs.size() << " exprs" << std::endl;

			return make_expr<UnitExpr>(line(token), exprs, weak);
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_UNITPARSER_H_ */
