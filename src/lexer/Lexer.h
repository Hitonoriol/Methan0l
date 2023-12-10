#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <sstream>
#include <locale>
#include <queue>
#include <iostream>
#include <iterator>

#include <lexer/Token.h>
#include <lexer/TokenType.h>

namespace mtl
{

enum class IntLiteral {
		NONE, HEX, BIN, DEC
};

class Lexer
{
	private:
		static const std::unordered_map<char, char> escape_seqs;
		static const std::string digits, double_digits;

		std::string::iterator cur_chr, input_end;

		std::string tokstr;
		TokenType toktype = Tokens::NONE;
		IntLiteral cur_int_literal = IntLiteral::NONE;

		uint32_t line = 1, column = 1;
		Separator cur_sep = Separator::NONE;
		Int open_blocks = 0;
		bool token_literal = false;

		std::queue<Token> tokens;

		std::unordered_set<TokenType> punctuators;
		std::vector<TokenType> single_char_ops;
		std::vector<TokenType> multichar_ops;
		std::vector<TokenType> soft_word_ops;
		std::vector<TokenType> hard_word_ops;
		std::vector<TokenType> reserved_words;

		std::vector<TokenType> block_begin_tokens;
		std::vector<TokenType> block_end_tokens;

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

		inline bool saving_string()
		{
			return toktype == Tokens::STRING
					|| toktype == Tokens::FORMAT_STRING
					|| toktype == Tokens::CHAR;
		}

		inline bool saving_number()
		{
			return toktype == Tokens::INTEGER || toktype == Tokens::DOUBLE;
		}

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
		Lexer(const Lexer&);
		Lexer& operator=(const Lexer&);
		virtual ~Lexer() = default;

		void register_punctuator(TokenType punctuator);
		void register_keyword(TokenType keyword, bool hard = false);
		void register_operator(TokenType op);
		void register_block_begin_token(TokenType tok);
		void register_block_end_token(TokenType tok);

		void lex(std::string &code, bool preserve_state = false);

		Token next(bool peek = false);
		Token& last();

		void reset(bool full = true);
		bool has_unclosed_blocks();
		bool empty();
		size_t size();

		TokenType get_multichar_op_type(std::string_view op);

		bool is_hard_keyword(std::string_view typestr);
		bool is_soft_keyword(std::string_view typestr);
		bool is_keyword(std::string_view typestr);

		bool is_block_begin(TokenType tok);
		bool is_block_end(TokenType tok);

		bool is_blank(char chr);
		char escape_seq(char seq);
		bool is_punctuator(char chr);
		bool is_separator(char chr);

		char to_chr(TokenType tok);
		TokenType to_tok(char chr);
};

} /* namespace mtl */

#endif /* LEXER_H_ */
