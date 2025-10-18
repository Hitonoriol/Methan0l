#ifndef SRC_EXPRESSION_PARSER_FORMATSTRPARSER_H_
#define SRC_EXPRESSION_PARSER_FORMATSTRPARSER_H_

#include <parser/expression/PrefixExprParser.h>

#include <regex>

#include <expression/FormatStrExpr.h>
#include <expression/LiteralExpr.h>
#include <util/StringFormatter.h>

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
			parser.match(Tokens::COMMA);
			ExprList args;
			if (StringFormatter::contains_format(token.get_value())) {
				do {
					args.push_back(parser.parse());
				} while (parser.match(Tokens::COMMA));
				return make_expr<FormatStrExpr>(line(token), strip_quotes(unconst(token.get_value())), args);
			}

			return make_expr<LiteralExpr>(line(token), token.get_value());
		}
};

}

#endif /* SRC_EXPRESSION_PARSER_FORMATSTRPARSER_H_ */
