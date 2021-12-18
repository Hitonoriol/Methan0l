#ifndef SRC_EXPRESSION_PARSER_EXPRPARSER_H_
#define SRC_EXPRESSION_PARSER_EXPRPARSER_H_

namespace mtl
{

class Token;

class ExprParser
{
	public:
		virtual ~ExprParser() = default;
		virtual int precedence() = 0;
		virtual bool is_compatible(const Token &tok) {return true;}
};

}

#endif /* SRC_EXPRESSION_PARSER_EXPRPARSER_H_ */
