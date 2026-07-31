#ifndef EXPRESSION_PARSER_INVOKEPARSER_H_
#define EXPRESSION_PARSER_INVOKEPARSER_H_

#include <ch3oh/parser/expression/InfixParser.h>
#include <methan0l/expression/ListExpr.h>
#include <methan0l/expression/parser/ListParser.h>
#include <methan0l/expression/InvokeExpr.h>

namespace mtl
{

class InvokeParser: public InfixParser
{
	public:
		ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) override
		{
			ExprList args;
			ListParser::parse(parser, [&](ExprPtr expr) {args.push_back(expr);});
			if (parser.match(Tokens::COLON)) {
				do {
					args.push_back(parser.parse());
				} while (parser.match(Tokens::COMMA));
			}
			return make_expr<InvokeExpr>(line(token), lhs, args);
		}

		int precedence()
		{
			return prcdc(Precedence::INVOKE);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_INVOKEPARSER_H_ */
