#ifndef EXPRESSION_PARSER_INFIXPARSER_H_
#define EXPRESSION_PARSER_INFIXPARSER_H_

#include "ExprParser.h"

#include <stdexcept>
#include <memory>

#include <ch3oh/lexer/Token.h>
#include <methan0l/expression/parser/precedence.h>
#include <methan0l/expression/IdentifierExpr.h>
#include <methan0l/util/util.h>

namespace mtl
{

class Expression;
class Parser;

class InfixParser: public ExprParser
{
	private:
		friend class Parser;

		bool ignore_nl_separated = false;

	protected:
		inline void ignore_newline_separated()
		{
			ignore_nl_separated = true;
		}

	public:
		virtual ~InfixParser() = default;
		virtual ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) = 0;
		inline bool is_compatible(const Token &tok) override
		{
			if constexpr (DEBUG)
				std::cout << "Ignoring NLs: " << ignore_nl_separated << " | "
						<< tok << " is first in line: " << tok.first_in_line() << std::endl;

			if (ignore_nl_separated && tok.first_in_line())
				return false;
			return true;
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_INFIXPARSER_H_ */
