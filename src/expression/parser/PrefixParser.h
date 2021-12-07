#ifndef EXPRESSION_PARSER_PREFIXPARSER_H_
#define EXPRESSION_PARSER_PREFIXPARSER_H_

#include "../../type.h"

namespace mtl
{

class Parser;

class PrefixParser
{
	protected:
		int prec;

	public:
		PrefixParser(int prec = 0) : prec(prec) {}
		virtual ExprPtr parse(Parser &parser, Token token) = 0;
		virtual ~PrefixParser() = default;
		int precedence() {return prec;}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_PREFIXPARSER_H_ */
