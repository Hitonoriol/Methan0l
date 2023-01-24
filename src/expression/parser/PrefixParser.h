#ifndef EXPRESSION_PARSER_PREFIXPARSER_H_
#define EXPRESSION_PARSER_PREFIXPARSER_H_

#include "type.h"
#include "precedence.h"
#include "ExprParser.h"

namespace mtl
{

class Parser;

class PrefixParser: public ExprParser
{
	protected:
		int prec;

	public:
		PrefixParser(int prec = 0) : prec(prec) {}
		PrefixParser(Precedence prec) : PrefixParser(prcdc(prec)) {}
		virtual ExprPtr parse(Parser &parser, Token token) = 0;
		virtual ~PrefixParser() = default;
		int precedence() override {return prec;}
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_PREFIXPARSER_H_ */
