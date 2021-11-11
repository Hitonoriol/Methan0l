#ifndef SRC_EXPRESSION_PARSER_LOOPPARSER_H_
#define SRC_EXPRESSION_PARSER_LOOPPARSER_H_

#include <memory>
#include <stdexcept>
#include <vector>

#include "../../Parser.h"
#include "../../type.h"
#include "../../Token.h"
#include "../../util/util.h"
#include "../ListExpr.h"
#include "../LoopExpr.h"
#include "PrefixParser.h"

namespace mtl
{

class LoopParser: public PrefixParser
{
	private:
		static const size_t FOR_ARGS = 3, FOREACH_ARGS = 2, WHILE_ARGS = 1;

	public:
		ExprPtr parse(Parser &parser, Token token) override
		{
			if constexpr (DEBUG)
				std::cout << "Parsing Loop Expression..." << std::endl;

			ExprList loop_params = try_cast<ListExpr>(parser.parse()).raw_list();
			ExprPtr body = parser.parse();
			size_t size = loop_params.size();
			uint32_t line = mtl::line(token);

			if (size == FOR_ARGS)
				return make_expr<LoopExpr>(line, loop_params, body);

			else if (size == FOREACH_ARGS)
				return make_expr<LoopExpr>(line, loop_params.front(), loop_params[1], body);

			else if (size == WHILE_ARGS)
				return make_expr<LoopExpr>(line, loop_params.back(), body);

			throw std::runtime_error("Invalid Loop parameter list expression quantity");
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_LOOPPARSER_H_ */
