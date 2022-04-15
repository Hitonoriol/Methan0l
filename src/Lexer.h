#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <sstream>
#include <locale>
#include <queue>
#include <iostream>
#include <iterator>

#include "type.h"
#include "Token.h"

namespace mtl
{

enum class IntLiteral {
		NONE, HEX, BIN, DEC
};

class Lexer
{
	private:
		std::string::iterator cur_chr, input_end;

		std::string tokstr;
		TokenType toktype = TokenType::NONE;
		IntLiteral cur_int_literal = IntLiteral::NONE;

		uint32_t line = 1, column = 1;
		Separator cur_sep = Separator::NONE;
		dec open_blocks = 0;
		bool token_literal = false;

		std::queue<Token> tokens;

		void clear();
		void consume_and_deduce();
		void consume();
		void save(char chr);
		void deduce_reserved();
		void deduce_word_op();
		void deduce_separator(char chr);
		void deduce_separator();
		void push();
		void push(char chr);
		void next_char(size_t n = 1);
		bool has_next();
		void begin(char chr);
		char look_ahead(size_t n = 1);
		bool try_save_multichar_op(char chr, char next);

		bool match_cur(TokenType tok);
		bool match_prev(TokenType tok);
		bool match_next(TokenType tok);
		bool match_next_punctuator(TokenType);

		bool unescaped(TokenType tok);
		bool escaped(TokenType tok);

		void new_line();
		Token &finalize_token(Token&&);

	public:
		Lexer();
		~Lexer() = default;
		void lex(std::string &code, bool preserve_state = false);

		Token next(bool peek = false);
		Token& last();

		void reset(bool full = true);
		bool has_unclosed_blocks();
		bool empty();
		size_t size();
};

} /* namespace mtl */

#endif /* LEXER_H_ */
