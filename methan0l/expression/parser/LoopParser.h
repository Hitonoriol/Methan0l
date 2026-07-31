#ifndef SRC_EXPRESSION_PARSER_LOOPPARSER_H_
#define SRC_EXPRESSION_PARSER_LOOPPARSER_H_

#include <ch3oh/parser/expression/PrefixExprParser.h>

#include "ListParser.h"
#include "UnitParser.h"

#include <memory>
#include <stdexcept>
#include <vector>

#include <ch3oh/lexer/Token.h>
#include <ch3oh/parser/Parser.h>
#include <methan0l/util/util.h>
#include <methan0l/expression/ListExpr.h>
#include <methan0l/expression/LoopExpr.h>

namespace mtl
{

/* <loop-keyword> <:> <> */
class LoopParser : public PrefixParser
{
	private:
		static const size_t FOR_ARGS = 3, FOREACH_ARGS = 2, WHILE_ARGS = 1;

	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			parser.consume(Tokens::COLON);
			parser.match(Tokens::PAREN_L); // Optionally consume `(`

			ExprList loop_params;
			ListParser::parse(parser, [&](auto expr) {loop_params.push_back(expr);}, Tokens::NONE);
			parser.match(Tokens::PAREN_R); // Optionally consume `)`

			auto body = UnitParser::parse_ctrl_block(parser, false);
			size_t size = loop_params.size();
			uint32_t line = mtl::line(token);

			if (token == Tokens::FOR) {
				if (size == FOR_ARGS)
					return make_expr<LoopExpr>(line, loop_params, body);

				else if (size == FOREACH_ARGS)
					return make_expr<LoopExpr>(line, loop_params.front(), loop_params[1], body);
			}
			else if ((token == Tokens::WHILE) && size == WHILE_ARGS)
				return make_expr<LoopExpr>(line, loop_params.back(), body);

			throw std::runtime_error("Invalid loop expression");
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_LOOPPARSER_H_ */
