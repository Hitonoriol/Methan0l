#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <sstream>
#include <locale>
#include <queue>
#include <iostream>
#include <iterator>

#include "Token.h"
#include "methan0l_type.h"

namespace mtl
{

class Lexer
{
	private:
		std::string::iterator cur_chr, input_end;
		std::string tokstr;
		TokenType toktype = TokenType::NONE;

		std::queue<Token> tokens;

		void clear();
		void consume();
		void consume(char chr);
		void save(char chr);
		void deduce_reserved();
		void push();
		void push(char chr);
		void next_char();
		bool has_next();
		void begin(char chr);
		char look_ahead(size_t n = 1);
		bool try_save_bichar_op(char chr, char next);

	public:
		Lexer();
		~Lexer() = default;
		void parse(std::string code);
		Token next();
		bool empty();
		size_t size();
};

} /* namespace mtl */

#endif /* LEXER_H_ */
