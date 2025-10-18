#ifndef EXPRESSION_PARSER_PREFIXPARSER_H_
#define EXPRESSION_PARSER_PREFIXPARSER_H_

#include "ExprParser.h"
#include "PrefixExpr.h"

#include <type.h>
#include <expression/parser/precedence.h>

namespace mtl
{

class Parser;

class PrefixParser : public ExprParser
{
public:
	PrefixParser(int prec = 0 /* Or Precedence::PREFIX? */) : prec(prec) {}
	PrefixParser(Precedence prec) : PrefixParser(prcdc(prec)) {}

	virtual ExprPtr parse(Parser& parser, Token token);

	int precedence() override { return prec; }
	
protected:
	int prec;
};

} /* namespace mtl */

#endif /* EXPRESSION_PARSER_PREFIXPARSER_H_ */
