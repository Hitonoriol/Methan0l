#include "Methan0lParser.h"

#include "Methan0lLexer.h"

#include <lexer/Token.h>
#include <expression/parser/AssignParser.h>
#include <expression/parser/IdentifierParser.h>
#include <expression/parser/GroupParser.h>
#include <expression/parser/InvokeParser.h>
#include <expression/parser/PrefixOperatorParser.h>
#include <expression/parser/ConditionParser.h>
#include <expression/parser/BinaryOperatorParser.h>
#include <expression/parser/LiteralParser.h>
#include <expression/parser/PostfixExprParser.h>
#include <expression/parser/ListParser.h>
#include <expression/parser/UnitParser.h>
#include <expression/parser/WeakUnitParser.h>
#include <expression/parser/IndexParser.h>
#include <expression/parser/LoopParser.h>
#include <expression/parser/MapParser.h>
#include <expression/parser/FunctionParser.h>
#include <expression/parser/BoxUnitParser.h>
#include <expression/parser/ClassParser.h>
#include <expression/parser/WordOperatorParser.h>
#include <expression/parser/InfixWordOperatorParser.h>
#include <expression/parser/TryCatchParser.h>
#include <expression/parser/FormatStrParser.h>
#include <expression/parser/RangeParser.h>
#include <expression/parser/ConstParser.h>
#include <expression/parser/IfElseParser.h>

namespace mtl
{

/* Grammar definition */
Methan0lParser::Methan0lParser(Interpreter &context)
	: Parser(context, std::make_unique<Methan0lLexer>())
{
	/* Literals */
	register_literal_parser(Tokens::BOOLEAN, Type::BOOLEAN);
	register_literal_parser(Tokens::STRING, Type::STRING);
	register_literal_parser(Tokens::DOUBLE, Type::DOUBLE);
	register_literal_parser(Tokens::INTEGER, Type::INTEGER);
	register_literal_parser(Tokens::CHAR, Type::CHAR);
	register_literal_parser(Tokens::TOKEN, Type::TOKEN);
	register_parser(Tokens::FORMAT_STRING, new FormatStrParser()); // $"... {} ..." arg1, arg2, ...
	register_parser(Tokens::DOUBLE_DOT, new RangeParser()); // Range expression: start..end[..stop]

	/* Identifier expressions */
	register_parser(Tokens::HASH, new IdentifierParser()); // #foo -- global scope lookup
	register_parser(Tokens::IDENTIFIER, new IdentifierParser());	// foo -- local scope

	/* Const expressions */
	register_parser(Tokens::CONST, new ConstParser()); // const: ...
	register_parser(Tokens::LIST, new ConstParser(Precedence::HIGHEST)); // $name

	/* Assignment */
	register_parser(Tokens::ASSIGN, new AssignParser());				// lhs = rhs
	alias_infix(Tokens::ASSIGN, Tokens::ARROW_L);					// lhs <- rhs
	register_infix_opr(Tokens::TYPE_ASSIGN, Precedence::ASSIGNMENT); // lhs := rhs

	/* Group */
	register_parser(Tokens::PAREN_L, new GroupParser());			// (a + b)
	alias_prefix(Tokens::PAREN_L, Tokens::INFIX_WORD_LHS_L);

	/* Function invocation
	 * foo$(arg1, arg2, ...)
	 * 		or
	 * foo(arg1, arg2, ...)
	 * 		or
	 * foo(arg1, ..., arg_n): arg_n+1, arg_n+2, ...
	 */
	register_parser(Tokens::LIST_DEF_L, new InvokeParser());
	alias_infix(Tokens::LIST_DEF_L, Tokens::PAREN_L);

	/* Collection definition expressions */
	register_parser(Tokens::LIST_DEF_L, new ListParser(Tokens::PAREN_R)); // $(expr1, expr2, ...)
	register_parser(Tokens::BRACKET_L, new ListParser(Tokens::BRACKET_R)); // [expr1, expr2, ...]
	alias_prefix(Tokens::LIST_DEF_L, Tokens::SET_DEF); // set: expr1, expr2, ...
	alias_prefix(Tokens::LIST_DEF_L, Tokens::LIST_DEF); // list: expr1, expr2, ...

	/* Map definition */
	register_parser(Tokens::MAP_DEF_L, new MapParser());
	alias_prefix(Tokens::MAP_DEF_L, Tokens::MAP_DEF_L_ALT);
	alias_prefix(Tokens::MAP_DEF_L, Tokens::MAP_DEF); // map: key => value, ...
	
	/* Unit definition expressions */
	register_parser(Tokens::BRACE_L, new UnitParser()); // {expr1; expr2; expr3}
	register_parser(Tokens::ARROW_R, new WeakUnitParser()); // ->{expr1; expr2; expr3}
	register_parser(Tokens::BOX, new BoxUnitParser()); // box_unit = box: {expr1, expr2, ...}

	/* Index operator */
	register_parser(Tokens::BRACKET_L, new IndexParser()); // list[expr] or list[]

	/* If-else syntax:
	 * if <condition> <then-branch> [else:] <else-branch>
	 */
	register_parser(Tokens::IF, new IfElseParser()); // $name
	register_parser(Tokens::QUESTION, new ConditionParser()); // (a && b ? "yep" : "nah")

	/* Loop syntax:
	 * 1. while: (condition_expr) {}
	 * 2. for: (init_expr, condition_expr, step_expr) {}
	 * 3. for: (as_name, iterable_expr) {}
	 */
	register_parser(Tokens::WHILE, new LoopParser());
	alias_prefix(Tokens::WHILE, Tokens::FOR);

	/* Try-catch */
	register_parser(Tokens::TRY, new TryCatchParser()); // try { ... } catch: ex_name { ... }

	/* Function definition. See `FunctionParser` header for details. */
	register_parser(Tokens::FUNC_DEF, new FunctionParser());
	alias_prefix(Tokens::FUNC_DEF, Tokens::FUNC_DEF_SHORT);
	alias_prefix(Tokens::FUNC_DEF, Tokens::METHOD);

	/* Class definition */
	register_parser(Tokens::CLASS, new ClassParser()); // class: ClassName = @(private => $(), ...)
	alias_prefix(Tokens::CLASS, Tokens::INTERFACE);

	/* Access operators */
	register_infix_opr(Tokens::AT, Precedence::DOT, BinOprType::RIGHT_ASSOC);
	register_infix_opr(Tokens::DOT, Precedence::DOT);

	/* IO / String oprs */
	register_prefix_opr(Tokens::OUT_NL, Precedence::IO);				// <% expr
	register_prefix_opr(Tokens::OUT, Precedence::IO);				// %% expr
	register_prefix_opr(Tokens::IN, Precedence::IO);					// %> expr
	register_infix_opr(Tokens::STRING_CONCAT, Precedence::STRING_CONCAT);// expr1 :: expr2 :: expr3
	register_prefix_opr(Tokens::DOUBLE_DOLLAR); // $$expr

	/* Return operators */
	register_postfix_opr(Tokens::EXCLAMATION, Precedence::IO);				// expr!
	register_word(Tokens::RETURN, Precedence::IO);					// return: expr

	/* Logical oprs */
	register_prefix_opr(Tokens::EXCLAMATION);
	register_infix_opr(Tokens::BIT_AND, Precedence::BIT_AND);
	register_infix_opr(Tokens::BIT_OR, Precedence::BIT_OR);
	register_infix_opr(Tokens::BIT_XOR, Precedence::BIT_XOR);
	register_infix_opr(Tokens::AND, Precedence::LOG_AND);
	register_infix_opr(Tokens::OR, Precedence::LOG_OR);
	register_infix_opr(Tokens::XOR, Precedence::LOG_AND);
	register_infix_opr(Tokens::SHIFT_L, Precedence::BIT_SHIFT);
	register_infix_opr(Tokens::SHIFT_R, Precedence::BIT_SHIFT);
	register_prefix_opr(Tokens::TILDE);

	/* Comparison oprs */
	register_infix_opr(Tokens::GREATER, Precedence::COMPARISON);
	register_infix_opr(Tokens::GREATER_OR_EQ, Precedence::COMPARISON);
	register_infix_opr(Tokens::LESS, Precedence::COMPARISON);
	register_infix_opr(Tokens::LESS_OR_EQ, Precedence::COMPARISON);
	register_infix_opr(Tokens::EQUALS, Precedence::EQUALS);
	register_infix_opr(Tokens::NOT_EQUALS, Precedence::EQUALS);

	/* Binary Arithmetic oprs */
	register_infix_opr(Tokens::PLUS, Precedence::SUM);
	register_infix_opr(Tokens::MINUS, Precedence::SUM);
	register_infix_opr(Tokens::ASTERISK, Precedence::MULTIPLICATION);
	register_infix_opr(Tokens::PERCENT, Precedence::MULTIPLICATION);
	register_infix_opr(Tokens::SLASH, Precedence::MULTIPLICATION);

	/* Compound Assignment */
	register_infix_opr(Tokens::ADD, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(Tokens::SUB, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(Tokens::MUL, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(Tokens::DIV, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(Tokens::COMP_AND, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(Tokens::COMP_OR, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(Tokens::COMP_XOR, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(Tokens::COMP_SHIFT_L, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(Tokens::COMP_SHIFT_R, Precedence::COMP_ASSIGNMENT);
	register_infix_opr(Tokens::COMP_MOD, Precedence::COMP_ASSIGNMENT);

	/* Unary Arithmetic oprs */
	register_prefix_opr(Tokens::MINUS);								// unary -
	register_prefix_opr(Tokens::INCREMENT, Precedence::PREFIX_INCREMENT);		// ++x
	register_postfix_opr(Tokens::INCREMENT, Precedence::POSTFIX_INCREMENT);		// x++
	register_prefix_opr(Tokens::DECREMENT, Precedence::PREFIX_INCREMENT);		// --x
	register_postfix_opr(Tokens::DECREMENT, Precedence::POSTFIX_INCREMENT);		// x--

	/* Prefix keyword operators */
	register_word(Tokens::TYPE_ID);
	register_word(Tokens::TYPE_NAME);
	register_word(Tokens::DELETE);
	register_word(Tokens::OBJECT_COPY);
	register_word(Tokens::HASHCODE);
	register_word(Tokens::NO_EVAL, Precedence::NO_EVAL);
	register_word(Tokens::USING_MODULE);
	alias_prefix(Tokens::USING_MODULE, Tokens::IMPORT_MODULE);
	register_word(Tokens::NEW, Precedence::POSTFIX_INCREMENT);
	register_word(Tokens::GLOBAL, Precedence::PREFIX, true);
	register_word(Tokens::DEREF);
	register_word(Tokens::IS_REF);
	register_parser(Tokens::VAR, new VarDefParser());
	register_word(Tokens::IDENTITY);

	/* Infix keyword operators */
	register_infix_word(Tokens::ASSERT, Precedence::NO_EVAL);
	register_infix_word(Tokens::INSTANCE_OF, Precedence::BIT_SHIFT);
	register_infix_word(Tokens::REQUIRE, Precedence::NO_EVAL);
	register_infix_word(Tokens::CONVERT, Precedence::BIT_SHIFT);
}

} /* namespace mtl */
