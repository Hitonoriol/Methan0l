#include "Lexer.h"
#include "util/util.h"

namespace mtl
{

Lexer::Lexer() : tokens()
{

}

void Lexer::clear()
{
	tokstr.clear();
	toktype = TokenType::NONE;
}

void Lexer::save(char chr)
{
	tokstr += chr;
}

void Lexer::deduce_reserved()
{
	Word reserved = Token::as_reserved(tokstr);
	int wrd_id = static_cast<int>(reserved);
	if (reserved == Word::NONE)
		return;

	/* Value Type literal -- "evaluates" immediately to its index (Integer) */
	const int type_start = static_cast<int>(Word::TYPE_ID_START);
	if (wrd_id >= type_start) {
		toktype = TokenType::INTEGER;
		tokstr = std::to_string(wrd_id - type_start);
	}

	/* Boolean literal */
	else if (reserved == Word::TRUE || reserved == Word::FALSE)
		toktype = TokenType::BOOLEAN;
}

void Lexer::deduce_word_op()
{
	TokenType opr = Token::as_word_op(tokstr);
	if (opr == TokenType::NONE)
		return;

	toktype = opr;
}

void Lexer::push()
{
	if (tokstr.empty())
		return;

	if (toktype == TokenType::IDENTIFIER) {
		deduce_reserved();
		deduce_word_op();
		if (Token::is_semantic(toktype)) {
			clear();
			return;
		}
	}

	else if (toktype == TokenType::CHAR) {
		char bs = Token::chr(TokenType::BACKSLASH);
		if (tokstr.size() >= 3 && tokstr[1] == bs && tokstr[2] != bs) {
			if constexpr (DEBUG)
				out << "Escape sequence \"" << tokstr << "\"" << std::endl;
			tokstr = str(Token::escape_seq(strip_quotes(tokstr)));
		}
	}

	if constexpr (DEBUG)
		std::cout << "[" << tokstr << "] ";

	tokens.push(Token(toktype, tokstr).set_line(line));
	clear();
}

void Lexer::push(char chr)
{
	if (!Token::is_delimiter(chr)) {
		tokens.push(Token(chr).set_line(line));

		if (Token::is_block_begin(chr))
			++open_blocks;
		else if (Token::is_block_end(chr))
			--open_blocks;

		if constexpr (DEBUG)
			std::cout << "[" << chr << "] ";
	}
	else
	if constexpr (DEBUG)
		std::cout << std::endl;
}

char Lexer::look_ahead(size_t n)
{
	return *std::next(cur_chr, n);
}

/* Test if current and next chars are a double char opr, push to token queue if true */
bool Lexer::try_save_bichar_op(char chr, char next)
{
	std::string bichar_op(2, chr);
	bichar_op[1] = next;
	TokenType bichar_type = Token::get_bichar_op_type(bichar_op);
	if (bichar_type == TokenType::NONE) {
		return false;
	}

	if (bichar_type == TokenType::BLOCK_COMMENT_L) {
		do {
			next_char();
		} while (*cur_chr != Token::chr(TokenType::ASTERISK) ||
				*std::next(cur_chr) != Token::chr(TokenType::SLASH));
		next_char();
		clear();
		return true;
	}

	tokstr = bichar_op;
	toktype = bichar_type;
	push();
	next_char();
	return true;
}

/* Assume token type by the first character */
void Lexer::begin(char chr)
{
	if (chr == TokenType::QUOTE || chr == TokenType::SINGLE_QUOTE) {
		toktype = (chr == TokenType::QUOTE) ? TokenType::STRING : TokenType::CHAR;
		save(chr);
	}

	else if (!std::isalnum(chr) && Token::is_punctuator(chr)) {
		char next = look_ahead();
		if (Token::is_punctuator(next))
			if (try_save_bichar_op(chr, next))
				return;

		push(chr);
		clear();
		return;
	}

	else if (std::isdigit(chr)) {
		toktype = TokenType::INTEGER;
		save(chr);
	}

	else if (std::isalpha(chr)) {
		toktype = TokenType::IDENTIFIER;
		save(chr);
	}
}

/* Continue consuming after the starting assumption */
void Lexer::consume()
{
	char chr = *cur_chr;

	if (toktype == TokenType::INTEGER && chr == TokenType::DOT) {
		if (!std::isdigit(*std::next(cur_chr))) {
			push();
			push(chr);
			return;
		}

		save(chr);
		toktype = TokenType::DOUBLE;
		return;
	}

	else if (toktype == TokenType::STRING || toktype == TokenType::CHAR) {
		/* Save `\` literally when reading a CHAR & ignore unescaped `\` when reading a STRING */
		if (toktype == TokenType::CHAR
				|| (chr != TokenType::BACKSLASH || escaped(TokenType::BACKSLASH)))
			save(chr);

		if ((toktype == TokenType::STRING && unescaped(TokenType::QUOTE))
				|| (toktype == TokenType::CHAR && unescaped(TokenType::SINGLE_QUOTE)))
			push();

		return;
	}

	else if (Token::is_punctuator(chr)) {
		push();
		begin(chr);
		return;
	}

	if (std::isalnum(chr) || chr == TokenType::UNDERSCORE)
		save(chr);

}

/* Consume chrs one-by-one, accumulating them in a buffer & deducing the token type */
void Lexer::consume_and_deduce()
{
	if (*cur_chr == '\n')
		++line;

	if (toktype == TokenType::NONE)
		begin(*cur_chr);
	else
		consume();

	next_char();
}

void Lexer::next_char()
{
	if (has_next())
		++cur_chr;
}

bool Lexer::has_next()
{
	return cur_chr != input_end;
}

inline bool Lexer::match_cur(TokenType tok)
{
	return *cur_chr == Token::chr(tok);
}

inline bool Lexer::match_prev(TokenType tok)
{
	return *std::prev(cur_chr) == tok;
}

inline bool Lexer::match_next(TokenType tok)
{
	return *std::next(cur_chr) == tok;
}

inline bool Lexer::unescaped(TokenType tok)
{
	return match_cur(tok) && !match_prev(TokenType::BACKSLASH);
}

inline bool Lexer::escaped(TokenType tok)
{
	return match_cur(tok) && match_prev(TokenType::BACKSLASH);
}

void Lexer::parse(std::string &code, bool preserve_state)
{
	if (!preserve_state) {
		line = 1;
		cur_chr = code.begin();
		input_end = code.end();
		std::queue<Token>().swap(tokens);
		open_blocks = 0;
	}

	if constexpr (DEBUG)
		std::cout << "Lexing the input..." << std::endl;

	while (has_next())
		consume_and_deduce();
	push();

	if constexpr (DEBUG)
		std::cout << '\n' << "* Lexing complete. " << tokens.size() << " tokens parsed." << std::endl;

	tokens.push(Token::EOF_TOKEN);
}

Token Lexer::next(bool peek)
{
	if (tokens.empty())
		return Token::EOF_TOKEN;

	Token token = tokens.front();

	if (!peek)
		tokens.pop();

	return token;
}

bool Lexer::has_unclosed_blocks()
{
	return open_blocks > 0;
}

bool Lexer::empty()
{
	return tokens.empty();
}

size_t Lexer::size()
{
	return tokens.size();
}

} /* namespace mtl */
