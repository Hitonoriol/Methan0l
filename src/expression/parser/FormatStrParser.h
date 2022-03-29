#ifndef SRC_EXPRESSION_PARSER_FORMATSTRPARSER_H_
#define SRC_EXPRESSION_PARSER_FORMATSTRPARSER_H_

#include "PrefixParser.h"

#include <regex>

#include "../FormatStrExpr.h"
#include "../LiteralExpr.h"
#include <lang/core/LibString.h>

namespace mtl
{

/* Formatted strings:
 * (1)	$"Text {} more text {2}" arg1, arg2, ...
 *
 * or with an optional comma right after the string literal:
 * (2)	$"...", arg1, arg2, ...
 */

class FormatStrParser: public PrefixParser
{
	public:
		inline ExprPtr parse(Parser &parser, Token token)
		{
			parser.match(TokenType::COMMA);
			ExprList args;
			if (std::regex_search(token.get_value(), LibString::string_fmt)) {
				do {
					args.push_back(parser.parse());
				} while (parser.match(TokenType::COMMA));
				return make_expr<FormatStrExpr>(line(token), strip_quotes(token.get_value()), args);
			}

			return make_expr<LiteralExpr>(line(token), token.get_value());
		}
};

}

#endif /* SRC_EXPRESSION_PARSER_FORMATSTRPARSER_H_ */
