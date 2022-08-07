#ifndef SRC_EXPRESSION_PARSER_WORDOPERATORPARSER_H_
#define SRC_EXPRESSION_PARSER_WORDOPERATORPARSER_H_

#include "PrefixOperatorParser.h"

namespace mtl
{

class WordOperatorParser: public PrefixOperatorParser
{
	private:
		TokenType word;
		bool multiple_args;

	public:
		WordOperatorParser(TokenType word, int prec = prcdc(Precedence::PREFIX), bool accept_multiple_args = false)
			: PrefixOperatorParser(prec), word(word), multiple_args(accept_multiple_args) {}
		ExprPtr parse(Parser &parser, Token token) override;
};

class VarDefParser: public PrefixOperatorParser
{
	public:
		VarDefParser() : PrefixOperatorParser(prcdc(Precedence::HIGHEST)) {}
		ExprPtr parse(Parser &parser, Token token) override;

};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_PARSER_WORDOPERATORPARSER_H_ */
