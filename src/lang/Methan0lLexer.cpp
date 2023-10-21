#include "Methan0lLexer.h"

namespace mtl
{

Methan0lLexer::Methan0lLexer()
{
	/* Punctuator tokens */
	register_punctuator(Tokens::PAREN_L);
	register_punctuator(Tokens::PAREN_R);
	register_punctuator(Tokens::BRACKET_L);
	register_punctuator(Tokens::BRACKET_R);
	register_punctuator(Tokens::BRACE_L);
	register_punctuator(Tokens::BRACE_R);
	register_punctuator(Tokens::COLON);
	register_punctuator(Tokens::SEMICOLON);
	register_punctuator(Tokens::COMMA);
	register_punctuator(Tokens::QUOTE);
	register_punctuator(Tokens::SINGLE_QUOTE);
	register_punctuator(Tokens::QUOTE_ALT);

	/* Operators */
	register_operator(Tokens::ASSIGN);
	register_operator(Tokens::PLUS);
	register_operator(Tokens::MINUS);
	register_operator(Tokens::SLASH);
	register_operator(Tokens::ASTERISK);
	register_operator(Tokens::BACKSLASH);
	register_operator(Tokens::BIT_XOR);
	register_operator(Tokens::EXCLAMATION);
	register_operator(Tokens::QUESTION);
	register_operator(Tokens::TILDE);
	register_operator(Tokens::GREATER);
	register_operator(Tokens::LESS);
	register_operator(Tokens::BIT_OR);
	register_operator(Tokens::BIT_AND);
	register_operator(Tokens::PERCENT);
	register_operator(Tokens::HASH);
	register_operator(Tokens::AT);
	register_operator(Tokens::LIST);
	register_operator(Tokens::DOT);

	/* Multi-character operators */
	register_operator(Tokens::SHIFT_L);
	register_operator(Tokens::SHIFT_R);
	register_operator(Tokens::ARROW_R);
	register_operator(Tokens::ARROW_L);
	register_operator(Tokens::LIST_DEF_L);
	register_operator(Tokens::MAP_DEF_L);
	register_operator(Tokens::EQUALS);
	register_operator(Tokens::OUT);
	register_operator(Tokens::STRING_CONCAT);
	register_operator(Tokens::BLOCK_COMMENT_L);
	register_operator(Tokens::BLOCK_COMMENT_R);
	register_operator(Tokens::GREATER_OR_EQ);
	register_operator(Tokens::LESS_OR_EQ);
	register_operator(Tokens::INCREMENT);
	register_operator(Tokens::DECREMENT);
	register_operator(Tokens::TYPE_ASSIGN);
	register_operator(Tokens::KEYVAL);
	register_operator(Tokens::NOT_EQUALS);
	register_operator(Tokens::ADD);
	register_operator(Tokens::SUB);
	register_operator(Tokens::MUL);
	register_operator(Tokens::DIV);
	register_operator(Tokens::AND);
	register_operator(Tokens::OR);
	register_operator(Tokens::XOR);
	register_operator(Tokens::IN);
	register_operator(Tokens::OUT_NL);
	register_operator(Tokens::INFIX_WORD_LHS_L);
	register_operator(Tokens::FUNC_DEF_SHORT_ALT);
	register_operator(Tokens::DOUBLE_DOLLAR);
	register_operator(Tokens::LONG_ARROW_RIGHT);
	register_operator(Tokens::COMP_XOR);
	register_operator(Tokens::COMP_OR);
	register_operator(Tokens::COMP_AND);
	register_operator(Tokens::COMP_SHIFT_L);
	register_operator(Tokens::COMP_SHIFT_R);
	register_operator(Tokens::COMP_MOD);
	register_operator(Tokens::DOUBLE_SLASH);
	register_operator(Tokens::DOUBLE_DOT);
	register_operator(Tokens::MAP_DEF_L_ALT);

	/* Soft keywords */
	register_keyword(Tokens::TYPE_ID);
	register_keyword(Tokens::DELETE);
	register_keyword(Tokens::RETURN);
	register_keyword(Tokens::OBJECT_COPY);
	register_keyword(Tokens::HASHCODE);
	register_keyword(Tokens::TYPE_NAME);
	register_keyword(Tokens::NO_EVAL);
	register_keyword(Tokens::SET_DEF);
	register_keyword(Tokens::USING_MODULE);
	register_keyword(Tokens::NEW);
	register_keyword(Tokens::GLOBAL);
	register_keyword(Tokens::ASSERT);
	register_keyword(Tokens::INSTANCE_OF);
	register_keyword(Tokens::DEREF); // TODO: remove
	register_keyword(Tokens::IS_REF); // TODO: remove
	register_keyword(Tokens::METHOD);
	register_keyword(Tokens::FUNC_DEF_SHORT);
	register_keyword(Tokens::VAR);
	register_keyword(Tokens::IMPORT_MODULE);
	register_keyword(Tokens::BASE_CLASS);
	register_keyword(Tokens::REQUIRE);
	register_keyword(Tokens::INTERFACE);
	register_keyword(Tokens::CONST);
	register_keyword(Tokens::CONVERT);
	register_keyword(Tokens::IMPLEMENT);
	register_keyword(Tokens::IDENTITY);

	/* Hard keywords */
	register_keyword(Tokens::DO, true);
	register_keyword(Tokens::FOR, true);
	register_keyword(Tokens::WHILE, true);
	register_keyword(Tokens::FUNC_DEF, true);
	register_keyword(Tokens::BOX, true);
	register_keyword(Tokens::CLASS, true);
	register_keyword(Tokens::IF, true);
	register_keyword(Tokens::ELSE, true);
	register_keyword(Tokens::SET_DEF, true);
	register_keyword(Tokens::TRY, true);
	register_keyword(Tokens::CATCH, true);

	/* Block begin tokens */
	register_block_begin_token(Tokens::PAREN_L);
	register_block_begin_token(Tokens::BRACKET_L);
	register_block_begin_token(Tokens::BRACE_L);
	register_block_begin_token(Tokens::MAP_DEF_L);
	register_block_begin_token(Tokens::MAP_DEF_L_ALT);
	register_block_begin_token(Tokens::LIST_DEF_L);
	register_block_begin_token(Tokens::INFIX_WORD_LHS_L);

	/* Block end tokens */
	register_block_end_token(Tokens::PAREN_R);
	register_block_end_token(Tokens::BRACKET_R);
	register_block_end_token(Tokens::BRACE_R);
}

} /* namespace mtl */
