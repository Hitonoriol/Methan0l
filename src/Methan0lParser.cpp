#include "Methan0lParser.h"

namespace mtl
{

Methan0lParser::Methan0lParser() : Parser(Lexer())
{
	register_literal_parser(TokenType::BOOLEAN, Type::BOOLEAN);
	register_literal_parser(TokenType::STRING, Type::STRING);
	register_literal_parser(TokenType::DOUBLE, Type::DOUBLE);
	register_literal_parser(TokenType::INTEGER, Type::INTEGER);

	register_parser(TokenType::IDENTIFIER, new IdentifierParser());		// foo
	register_parser(TokenType::ASSIGN, new AssignParser());				// lhs = rhs
	register_parser(TokenType::QUESTION, new ConditionParser());// (a & b ? "yep" : "nah")
	register_parser(TokenType::PAREN_L, new GroupParser());			// x * ((a + b) + c)
	register_parser(TokenType::LIST_DEF_L, new InvokeParser());	// some_func $(arg1, arg2, arg3)
	register_parser(TokenType::LIST_DEF_L, new ListParser());	// $(arg1, arg2, arg3)
	register_parser(TokenType::BRACE_L, new UnitParser());	// {expr1; expr2; expr3}
	register_prefix_opr(TokenType::CONCAT);								// <<$(a, b, c)
	register_prefix_opr(TokenType::OUT);								// %%foo
	register_prefix_opr(TokenType::INPUT);								// >>foo
	register_infix_opr(TokenType::INLINE_CONCAT, Precedence::SUM);// expr1 :: expr2 :: expr3

	register_postfix_opr(TokenType::EXCLAMATION);						// return opr

	register_prefix_opr(TokenType::MINUS);								// unary -
	register_prefix_opr(TokenType::INCREMENT);								// ++x
	register_postfix_opr(TokenType::INCREMENT);							// x++
	register_prefix_opr(TokenType::DECREMENT);							// --x
	register_postfix_opr(TokenType::DECREMENT);							// x--

	register_infix_opr(TokenType::GREATER, Precedence::CONDITIONAL);
	register_infix_opr(TokenType::GREATER_OR_EQ, Precedence::CONDITIONAL);
	register_infix_opr(TokenType::LESS, Precedence::CONDITIONAL);
	register_infix_opr(TokenType::LESS_OR_EQ, Precedence::CONDITIONAL);
	register_infix_opr(TokenType::EQUALS, Precedence::MULTIPLICATION);

	register_prefix_opr(TokenType::EXCLAMATION);
	register_infix_opr(TokenType::AND, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::PIPE, Precedence::SUM);

	register_infix_opr(TokenType::PLUS, Precedence::SUM);
	register_infix_opr(TokenType::MINUS, Precedence::SUM);
	register_infix_opr(TokenType::ASTERISK, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::PERCENT, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::SLASH, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::POWER, Precedence::EXPONENT, BinOprType::RIGHT_ASSOC);

}

void Methan0lParser::register_literal_parser(TokenType token, Type val_type)
{
	register_parser(token, new LiteralParser(val_type));
}

void Methan0lParser::register_prefix_opr(TokenType token, int precedence)
{
	register_parser(token, new PrefixOperatorParser(precedence));
}

void Methan0lParser::register_postfix_opr(TokenType token, int precedence)
{
	register_parser(token, new PostfixExprParser(precedence));
}

void Methan0lParser::register_infix_opr(TokenType token, Precedence precedence,
		BinOprType type)
{
	register_parser(token,
			new BinaryOperatorParser(static_cast<int>(precedence),
					type == BinOprType::RIGHT_ASSOC));
}

void Methan0lParser::load(std::string &code)
{
	lexer.parse(code);
	parse_all();
}

} /* namespace mtl */
