#ifndef SRC_EXPRESSION_PARSER_LOOPPARSER_H_
#define SRC_EXPRESSION_PARSER_LOOPPARSER_H_

#include "PrefixParser.h"

#include <memory>
#include <stdexcept>
#include <vector>

#include <lexer/Token.h>
#include <parser/Parser.h>
#include <util/util.h>
#include <expression/ListExpr.h>
#include <expression/LoopExpr.h>

namespace mtl
{

class LoopParser: public PrefixParser
{
	private:
		static const size_t FOR_ARGS = 3, FOREACH_ARGS = 2, WHILE_ARGS = 1;

	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			if (!parser.match(TokenType::LIST_DEF_L))
				parser.consume(TokenType::PAREN_L);

			ExprList loop_params;
			ListParser::parse(parser, [&](auto expr) {loop_params.push_back(expr);});
			ExprPtr body = UnitParser::parse_ctrl_block(parser, false);
			size_t size = loop_params.size();
			uint32_t line = mtl::line(token);
			bool legacy_loop = token == TokenType::DO;

			if (token == TokenType::FOR || legacy_loop) {
				if (size == FOR_ARGS)
					return make_expr<LoopExpr>(line, loop_params, body);

				else if (size == FOREACH_ARGS)
					return make_expr<LoopExpr>(line, loop_params.front(), loop_params[1], body);
			}
			else if ((token == TokenType::WHILE || legacy_loop) && size == WHILE_ARGS)
				return make_expr<LoopExpr>(line, loop_params.back(), body);

			throw std::runtime_error("Invalid loop expression");
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_LOOPPARSER_H_ */
