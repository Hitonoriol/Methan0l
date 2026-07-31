#ifndef SRC_EXPRESSION_PARSER_PRECEDENCE_H_
#define SRC_EXPRESSION_PARSER_PRECEDENCE_H_

namespace mtl
{

enum class Precedence
{
	NO_EVAL = 1,		// noeval: expr
	IO,					// <% %% %>
	COMP_ASSIGNMENT,	// += -= *= /=
	ASSIGNMENT,			// = ->
	STRING_CONCAT,		// :: e1..e2
	CONDITIONAL,		// e1 ? e2 : e3
	LOG_OR,				// ||
	LOG_AND,			// &&
	BIT_OR,				// |
	BIT_XOR,			// ^
	BIT_AND,			// &
	EQUALS,				// == !=
	COMPARISON,			// > < <= >=
	BIT_SHIFT,			// >> <<
	SUM,				// + -
	MULTIPLICATION,		// * / %
	PREFIX,				// - ! ~
	PREFIX_INCREMENT,	// ++expr --expr
	POSTFIX,			//
	DOT,				// . @
	INDEX,				// []
	INVOKE,				// ()
	POSTFIX_INCREMENT,	// expr++ / expr--
	HIGHEST				// $const; var: x = ...
};

constexpr int prcdc(Precedence prec)
{
	return static_cast<int>(prec);
}

inline int operator -(const Precedence &lhs, int rhs)
{
	return prcdc(lhs) - rhs;
}

}

#endif /* SRC_EXPRESSION_PARSER_PRECEDENCE_H_ */
