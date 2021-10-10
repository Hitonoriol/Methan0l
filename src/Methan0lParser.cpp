#include "Methan0lParser.h"

#include "expression/parser/AssignParser.h"
#include "expression/parser/IdentifierParser.h"
#include "expression/parser/GroupParser.h"
#include "expression/parser/InvokeParser.h"
#include "expression/parser/PrefixOperatorParser.h"
#include "expression/parser/ConditionParser.h"
#include "expression/parser/BinaryOperatorParser.h"
#include "expression/parser/LiteralParser.h"
#include "expression/parser/PostfixExprParser.h"
#include "expression/parser/ListParser.h"
#include "expression/parser/UnitParser.h"
#include "expression/parser/WeakUnitParser.h"
#include "expression/parser/IndexParser.h"
#include "expression/parser/LoopParser.h"
#include "expression/parser/MapParser.h"
#include "expression/parser/FunctionParser.h"
#include "expression/parser/BoxUnitParser.h"

namespace mtl
{

/* Grammar definition */
Methan0lParser::Methan0lParser() : Parser(Lexer())
{
	/* Literals */
	register_literal_parser(TokenType::BOOLEAN, Type::BOOLEAN);
	register_literal_parser(TokenType::STRING, Type::STRING);
	register_literal_parser(TokenType::DOUBLE, Type::DOUBLE);
	register_literal_parser(TokenType::INTEGER, Type::INTEGER);

	/* Core syntax constructs */
	register_parser(TokenType::HASH, new IdentifierParser()); // #foo -- global scope lookup
	register_parser(TokenType::IDENTIFIER, new IdentifierParser());	// foo -- local scope
	register_parser(TokenType::ASSIGN, new AssignParser());				// lhs = rhs
	register_parser(TokenType::QUESTION, new ConditionParser());// (a & b ? "yep" : "nah") // else expr is optional
	register_parser(TokenType::PAREN_L, new GroupParser());			// x * ((a + b) + c)
	register_parser(TokenType::LIST_DEF_L, new InvokeParser());	// some_func $(arg1, arg2, arg3)
	register_parser(TokenType::LIST_DEF_L, new ListParser());	// $(arg1, arg2, arg3)
	register_parser(TokenType::BRACE_L, new UnitParser());	// {expr1; expr2; expr3}
	register_parser(TokenType::WEAK_UNIT_DEF, new WeakUnitParser());// ->{expr1; expr2; expr3}
	register_parser(TokenType::BRACKET_L, new IndexParser());	// list[expr] or list[]
	register_parser(TokenType::DO, new LoopParser());	// do $(i = 0, i < n, ++i) -> {}
	register_parser(TokenType::FUNC_DEF, new FunctionParser());	// func def @(arg1, arg2: def_val, ...) {expr1, expr2, ...}
	register_parser(TokenType::BOX, new BoxUnitParser());// box_unit = box {expr1, expr2, ...}

	/* Map definition */
	register_parser(TokenType::MAP_DEF_L, new MapParser());
	register_infix_opr(TokenType::KEYVAL, Precedence::SUM);

	/* IO / String oprs */
	register_prefix_opr(TokenType::SHIFT_L);								// <<$(a, b, c)
	register_prefix_opr(TokenType::OUT);								// %%foo
	register_prefix_opr(TokenType::SHIFT_R);								// >>foo
	register_infix_opr(TokenType::INLINE_CONCAT, Precedence::SUM);// expr1 :: expr2 :: expr3

	/* Return opr */
	register_postfix_opr(TokenType::EXCLAMATION);						// return opr

	/* Logical oprs */
	register_prefix_opr(TokenType::EXCLAMATION);
	register_infix_opr(TokenType::BIT_AND, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::BIT_OR, Precedence::SUM);
	register_infix_opr(TokenType::BIT_XOR, Precedence::EXPONENT, BinOprType::RIGHT_ASSOC);
	register_infix_opr(TokenType::AND, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::OR, Precedence::SUM);
	register_infix_opr(TokenType::XOR, Precedence::SUM);
	register_infix_opr(TokenType::SHIFT_L, Precedence::SUM);
	register_infix_opr(TokenType::SHIFT_R, Precedence::SUM);
	register_prefix_opr(TokenType::TILDE);

	/* Comparison oprs */
	register_infix_opr(TokenType::GREATER, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::GREATER_OR_EQ, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::LESS, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::LESS_OR_EQ, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::EQUALS, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::NOT_EQUALS, Precedence::MULTIPLICATION);

	/* Binary Arithmetic oprs */
	register_infix_opr(TokenType::PLUS, Precedence::SUM);
	register_infix_opr(TokenType::MINUS, Precedence::SUM);
	register_infix_opr(TokenType::ASTERISK, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::PERCENT, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::SLASH, Precedence::MULTIPLICATION);

	/* +=, -=, /=, *= */
	register_infix_opr(TokenType::ADD, Precedence::ASSIGNMENT);
	register_infix_opr(TokenType::SUB, Precedence::ASSIGNMENT);
	register_infix_opr(TokenType::MUL, Precedence::ASSIGNMENT);
	register_infix_opr(TokenType::DIV, Precedence::ASSIGNMENT);

	/* Unary Arithmetic oprs */
	register_prefix_opr(TokenType::MINUS);								// unary -
	register_prefix_opr(TokenType::INCREMENT);								// ++x
	register_postfix_opr(TokenType::INCREMENT);							// x++
	register_prefix_opr(TokenType::DECREMENT);							// --x
	register_postfix_opr(TokenType::DECREMENT);							// x--

	/* Word oprs */
	register_prefix_opr(TokenType::TYPE);
	register_prefix_opr(TokenType::DELETE);
	register_prefix_opr(TokenType::DO);
	register_prefix_opr(TokenType::EXIT);
	register_prefix_opr(TokenType::LOAD);
	register_prefix_opr(TokenType::PERSISTENT);
	register_infix_opr(TokenType::DOT, Precedence::EXPONENT);

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

void Methan0lParser::load(const std::string &code)
{
	lexer.parse(code);
	parse_all();
}

} /* namespace mtl */
