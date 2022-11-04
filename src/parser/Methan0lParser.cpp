#include <parser/Methan0lParser.h>
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
#include "expression/parser/ClassParser.h"
#include "expression/parser/WordOperatorParser.h"
#include "expression/parser/InfixWordOperatorParser.h"
#include "expression/parser/TryCatchParser.h"
#include "expression/parser/FormatStrParser.h"
#include "expression/parser/RangeParser.h"

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
	register_literal_parser(TokenType::CHAR, Type::CHAR);
	register_literal_parser(TokenType::TOKEN, Type::TOKEN);
	register_parser(TokenType::FORMAT_STRING, new FormatStrParser()); // $"... {} ..." arg1, arg2, ...
	register_parser(TokenType::DOUBLE_DOT, new RangeParser());

	/* Core syntax constructs */
	register_parser(TokenType::HASH, new IdentifierParser()); // #foo -- global scope lookup
	register_parser(TokenType::IDENTIFIER, new IdentifierParser());	// foo -- local scope
	register_prefix_opr(TokenType::PERCENT, Precedence::PREFIX);
	register_parser(TokenType::QUESTION, new ConditionParser());// (a && b ? "yep" : "nah")

	/* Assignment */
	register_parser(TokenType::ASSIGN, new AssignParser());				// lhs = rhs
	alias_infix(TokenType::ASSIGN, TokenType::ARROW_L);					// lhs <- rhs
	register_infix_opr(TokenType::TYPE_ASSIGN, Precedence::ASSIGNMENT); // lhs := rhs

	/* Group */
	register_parser(TokenType::PAREN_L, new GroupParser());			// (a + b)
	alias_prefix(TokenType::PAREN_L, TokenType::INFIX_WORD_LHS_L);

	/* Function invocation
	 * foo$(arg1, arg2, ...)
	 * 		or
	 * foo(arg1, arg2, ...)	*/
	register_parser(TokenType::LIST_DEF_L, new InvokeParser());
	alias_infix(TokenType::LIST_DEF_L, TokenType::PAREN_L);

	register_parser(TokenType::LIST_DEF_L, new ListParser(TokenType::PAREN_R));	// $(expr1, expr2, ...)
	register_parser(TokenType::BRACKET_L, new ListParser(TokenType::BRACKET_R));	// [expr1, expr2, ...]
	alias_prefix(TokenType::LIST_DEF_L, TokenType::SET_DEF);	// defset $(expr1, expr2, ...)
	register_parser(TokenType::BRACE_L, new UnitParser());	// {expr1; expr2; expr3}
	register_parser(TokenType::ARROW_R, new WeakUnitParser());	// ->{expr1; expr2; expr3}
	register_parser(TokenType::BRACKET_L, new IndexParser());	// list[expr] or list[]

	register_parser(TokenType::DO, new LoopParser());// do $(init_expr, condition_expr, step_expr) -> {}
	alias_prefix(TokenType::DO, TokenType::WHILE);
	alias_prefix(TokenType::DO, TokenType::FOR);

	register_parser(TokenType::FUNC_DEF, new FunctionParser());	// func @(arg1, arg2: def_val, ...) {expr1, expr2, ...}
	alias_prefix(TokenType::FUNC_DEF, TokenType::FUNC_DEF_SHORT);
	alias_prefix(TokenType::FUNC_DEF, TokenType::FUNC_DEF_SHORT_ALT);
	alias_prefix(TokenType::FUNC_DEF, TokenType::METHOD);

	register_parser(TokenType::BOX, new BoxUnitParser());// box_unit = box {expr1, expr2, ...}
	register_parser(TokenType::CLASS, new ClassParser()); // class: ClassName = @(private => $(), ...)
	register_parser(TokenType::TRY, new TryCatchParser());

	/* Reference operator */
	register_prefix_opr(TokenType::REF); // ref = **idfr

	/* Map definition */
	register_parser(TokenType::MAP_DEF_L, new MapParser());

	/* IO / String oprs */
	register_prefix_opr(TokenType::OUT_NL, Precedence::IO);				// <% expr
	register_prefix_opr(TokenType::OUT, Precedence::IO);				// %% expr
	register_prefix_opr(TokenType::IN, Precedence::IO);					// %> expr
	register_infix_opr(TokenType::STRING_CONCAT, Precedence::STRING_CONCAT);// expr1 :: expr2 :: expr3
	register_prefix_opr(TokenType::DOUBLE_DOLLAR); // $$expr

	/* Return operators */
	register_postfix_opr(TokenType::EXCLAMATION, Precedence::IO);				// expr!
	register_word(TokenType::RETURN, Precedence::IO);					// return: expr

	register_postfix_opr(TokenType::DOUBLE_EXCL);			// expr!!

	/* Logical oprs */
	register_prefix_opr(TokenType::EXCLAMATION);
	register_infix_opr(TokenType::BIT_AND, Precedence::BIT_AND);
	register_infix_opr(TokenType::BIT_OR, Precedence::BIT_OR);
	register_infix_opr(TokenType::BIT_XOR, Precedence::BIT_XOR);
	register_infix_opr(TokenType::AND, Precedence::LOG_AND);
	register_infix_opr(TokenType::OR, Precedence::LOG_OR);
	register_infix_opr(TokenType::XOR, Precedence::LOG_AND);
	register_infix_opr(TokenType::SHIFT_L, Precedence::BIT_SHIFT);
	register_infix_opr(TokenType::SHIFT_R, Precedence::BIT_SHIFT);
	register_prefix_opr(TokenType::TILDE);

	/* Comparison oprs */
	register_infix_opr(TokenType::GREATER, Precedence::COMPARISON);
	register_infix_opr(TokenType::GREATER_OR_EQ, Precedence::COMPARISON);
	register_infix_opr(TokenType::LESS, Precedence::COMPARISON);
	register_infix_opr(TokenType::LESS_OR_EQ, Precedence::COMPARISON);
	register_infix_opr(TokenType::EQUALS, Precedence::EQUALS);
	register_infix_opr(TokenType::NOT_EQUALS, Precedence::EQUALS);

	/* Binary Arithmetic oprs */
	register_infix_opr(TokenType::PLUS, Precedence::SUM);
	register_infix_opr(TokenType::MINUS, Precedence::SUM);
	register_infix_opr(TokenType::ASTERISK, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::PERCENT, Precedence::MULTIPLICATION);
	register_infix_opr(TokenType::SLASH, Precedence::MULTIPLICATION);

	/* Compound Assignment */
	register_infix_opr(TokenType::ADD, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(TokenType::SUB, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(TokenType::MUL, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(TokenType::DIV, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(TokenType::COMP_AND, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(TokenType::COMP_OR, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(TokenType::COMP_XOR, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(TokenType::COMP_SHIFT_L, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(TokenType::COMP_SHIFT_R, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(TokenType::COMP_MOD, Precedence::COMP_ASSIGNMENT);

	/* Unary Arithmetic oprs */
	register_prefix_opr(TokenType::MINUS);								// unary -
	register_prefix_opr(TokenType::INCREMENT, Precedence::PREFIX_INCREMENT);		// ++x
	register_postfix_opr(TokenType::INCREMENT, Precedence::POSTFIX_INCREMENT);		// x++
	register_prefix_opr(TokenType::DECREMENT, Precedence::PREFIX_INCREMENT);		// --x
	register_postfix_opr(TokenType::DECREMENT, Precedence::POSTFIX_INCREMENT);		// x--

	/* Word oprs */
	register_word(TokenType::TYPE_ID);
	register_word(TokenType::TYPE_NAME);
	register_word(TokenType::DELETE);
	register_word(TokenType::DEFINE_VALUE);
	register_word(TokenType::OBJECT_COPY);
	register_word(TokenType::HASHCODE);
	register_word(TokenType::NO_EVAL, Precedence::NO_EVAL);
	register_word(TokenType::USING_MODULE);
	alias_prefix(TokenType::USING_MODULE, TokenType::IMPORT_MODULE);
	register_word(TokenType::NEW, Precedence::POSTFIX_INCREMENT);
	register_word(TokenType::GLOBAL, Precedence::PREFIX, true);
	register_word(TokenType::DEREF);
	register_word(TokenType::IS_REF);
	register_parser(TokenType::VAR, new VarDefParser());

	/* Infix word oprs */
	register_infix_word(TokenType::TYPE_SAFE, Precedence::NO_EVAL);
	register_infix_word(TokenType::ASSERT, Precedence::NO_EVAL);
	register_infix_word(TokenType::INSTANCE_OF, Precedence::BIT_SHIFT);

	/* Class / Box field / method access operators */
	register_infix_opr(TokenType::AT, Precedence::DOT, BinOprType::RIGHT_ASSOC);
	register_infix_opr(TokenType::DOT, Precedence::DOT);
}

void Methan0lParser::register_word(TokenType wordop, Precedence prec, bool multiarg)
{
	register_parser(wordop, new WordOperatorParser(wordop, prcdc(prec), multiarg));
}

void Methan0lParser::register_infix_word(TokenType wordop, Precedence prec, BinOprType type)
{
	register_parser(wordop, new InfixWordOperatorParser(prcdc(prec), type == BinOprType::RIGHT_ASSOC));
}

void Methan0lParser::register_literal_parser(TokenType token, Type val_type)
{
	register_parser(token, new LiteralParser(val_type));
}

void Methan0lParser::register_prefix_opr(TokenType token, Precedence precedence)
{
	register_parser(token, new PrefixOperatorParser(prcdc(precedence)));
}

void Methan0lParser::register_postfix_opr(TokenType token, Precedence precedence)
{
	register_parser(token, new PostfixExprParser(prcdc(precedence)));
}

void Methan0lParser::register_infix_opr(TokenType token, Precedence precedence,
		BinOprType type)
{
	register_parser(token,
			new BinaryOperatorParser(prcdc(precedence), type == BinOprType::RIGHT_ASSOC));
}

void Methan0lParser::load(std::string &code)
{
	lexer.lex(code);
	parse_all();
}

} /* namespace mtl */