#ifndef EXPRESSION_PARSER_INFIXPARSER_H_
#define EXPRESSION_PARSER_INFIXPARSER_H_

#include <stdexcept>
#include <memory>

#include "../../type.h"
#include "../../util/util.h"
#include "../../Token.h"
#include "../IdentifierExpr.h"
#include "precedence.h"

namespace mtl
{
class Expression;
class Parser;
} /* namespace mtl */

namespace mtl
{

class InfixParser
{
	public:
		virtual ~InfixParser() = default;
		virtual ExprPtr parse(Parser &parser, ExprPtr lhs, Token token) = 0;
		virtual int precedence() = 0;
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_INFIXPARSER_H_ */
