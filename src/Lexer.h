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

class Lexer
{
	private:
		std::string::iterator cur_chr, input_end;
		std::string tokstr;
		TokenType toktype = TokenType::NONE;
		uint32_t line;
		dec open_blocks;

		std::queue<Token> tokens;

		void clear();
		void consume_and_deduce();
		void consume();
		void save(char chr);
		void deduce_reserved();
		void deduce_word_op();
		void push();
		void push(char chr);
		void next_char();
		bool has_next();
		void begin(char chr);
		char look_ahead(size_t n = 1);
		bool try_save_bichar_op(char chr, char next);

		bool match_cur(TokenType tok);
		bool match_prev(TokenType tok);
		bool match_next(TokenType tok);

		bool unescaped(TokenType tok);
		bool escaped(TokenType tok);

	public:
		Lexer();
		~Lexer() = default;
		void parse(std::string &code, bool preserve_state = false);
		Token next(bool peek = false);
		bool has_unclosed_blocks();
		bool empty();
		size_t size();
};

} /* namespace mtl */

#endif /* LEXER_H_ */
