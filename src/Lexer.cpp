#include "Lexer.h"

#include <bitset>

#include "util/util.h"

namespace mtl
{

Lexer::Lexer() : tokens()
{
}

udec bin_to_int(const std::string &binstr)
{
	if (binstr.empty())
		return 0;

	udec val = 0;
	for (char c : binstr) {
		val <<= 1;
		val += c - '0';
	}
	return val;
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

	if (Token::is_block_begin(toktype))
		++open_blocks;

	else if (toktype == TokenType::IDENTIFIER) {
		deduce_reserved();
		deduce_word_op();
	}

	else if (toktype == TokenType::CHAR) {
		char bs = Token::chr(TokenType::BACKSLASH);
		if (tokstr.size() >= 3 && tokstr[1] == bs && tokstr[2] != bs) {
			if constexpr (DEBUG)
				out << "Escape sequence \"" << tokstr << "\"" << std::endl;
			tokstr = str(Token::escape_seq(strip_quotes(tokstr)));
		}
	}

	else if (toktype == TokenType::INTEGER && cur_int_literal != IntLiteral::DEC) {
		udec val;
		switch (cur_int_literal) {
		case IntLiteral::HEX:
			std::istringstream(tokstr) >> std::hex >> val;
			tokstr.clear();
			break;

		case IntLiteral::BIN:
			val = bin_to_int(tokstr);
			tokstr.clear();
			break;

		default:
			break;
		}

		if (tokstr.empty())
			tokstr = std::move(std::to_string(val));
	}

	if constexpr (DEBUG)
		std::cout << "[" << static_cast<int>(cur_sep) << "|" << tokstr << "] ";

	tokens.push(finalize_token(Token(toktype, tokstr)));
	clear();
}

void Lexer::push(char chr)
{
	if (chr != TokenType::SEMICOLON && chr != TokenType::NEWLINE) {
		tokens.push(finalize_token(Token(chr)));

		if (Token::is_block_begin(Token::tok(chr)))
			++open_blocks;
		else if (Token::is_block_end(chr))
			--open_blocks;

		if constexpr (DEBUG)
			std::cout << "[" << static_cast<int>(cur_sep) << "|" << chr << "] ";
	}
	else
	if constexpr (DEBUG)
		std::cout << std::endl;
}

Token& Lexer::finalize_token(Token &&tok)
{
	tok.set_line(line);
	tok.set_column(column);
	tok.set_separator(cur_sep);
	cur_sep = Separator::NONE;
	return tok;
}

char Lexer::look_ahead(size_t n)
{
	return *std::next(cur_chr, n);
}

/*
 * Test if current and next n chars are a multi-char opr, push to token queue if true,
 * or handle block comments.
 */
bool Lexer::try_save_multichar_op(char chr, char next)
{
	std::string op(2, chr);
	op[1] = next;

	/* If characters after `next` are also punctuators, append them to this multi-char opr */
	for (auto it = std::next(cur_chr, 2); it != input_end && Token::is_punctuator(*it); ++it)
		op += *it;

	/* Test from longest to shortest possible punctuator combination */
	TokenType multichar_type;
	while(op.length() >= 2) {
		if ((multichar_type = Token::get_multichar_op_type(op)) != TokenType::NONE)
			break;
		op.erase(op.length() - 1);
	}

	if (multichar_type == TokenType::NONE)
		return false;

	/* Skip everything inside a block comment */
	if (multichar_type == TokenType::BLOCK_COMMENT_L) {
		do {
			if (*cur_chr == '\n')
				new_line();

			next_char();
		} while (*cur_chr != Token::chr(TokenType::ASTERISK) ||
				*std::next(cur_chr) != Token::chr(TokenType::SLASH));
		next_char();
		clear();
		return true;
	}

	tokstr = op;
	toktype = multichar_type;
	push();
	next_char();
	return true;
}

/* Assume token type by the first character */
void Lexer::begin(char chr)
{
	cur_int_literal = IntLiteral::NONE;
	/* Formatted String literal: $"..." */
	if (chr == TokenType::LIST && match_next(TokenType::QUOTE)) {
		toktype = TokenType::FORMAT_STRING;
		next_char();
		begin(*cur_chr);
	}

	/* String / Char literal */
	else if (chr == TokenType::QUOTE || chr == TokenType::SINGLE_QUOTE) {
		if (toktype != TokenType::FORMAT_STRING)
			toktype = (chr == TokenType::QUOTE) ? TokenType::STRING : TokenType::CHAR;
		save(chr);
	}

	/* Punctuator / Compound punctuator token */
	else if (!std::isalnum(chr) && Token::is_punctuator(chr)) {
		char next = look_ahead();
		if (Token::is_punctuator(next))
			if (try_save_multichar_op(chr, next))
				return;

		push(chr);
		clear();
		return;
	}

	else if (std::isdigit(chr)) {
		toktype = TokenType::INTEGER;
		save(chr);
	}

	else if (std::isalpha(chr) || chr == TokenType::UNDERSCORE) {
		toktype = TokenType::IDENTIFIER;
		save(chr);
	}
}

/* Continue consuming after the starting assumption */
void Lexer::consume()
{
	char chr = *cur_chr;

	/* Deduce floating point literal */
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

	/* Deduce integral literal type */
	else if (toktype == TokenType::INTEGER && cur_int_literal == IntLiteral::NONE) {
		/* A regular Decimal literal */
		if (!std::isalpha(chr) || *std::prev(cur_chr) != '0') {
			cur_int_literal = IntLiteral::DEC;
			if (isdigit(chr))
				save(chr);
			else
				consume();
			return;
		}

		tokstr.clear();
		switch (chr) {
		case 'X':
			case 'x':
			cur_int_literal = IntLiteral::HEX;
			return;

		case 'B':
			case 'b':
			cur_int_literal = IntLiteral::BIN;
			return;

		default:
			throw std::runtime_error("Invalid integral literal id");
		}
	}

	/* Save String / Character literal char */
	else if (toktype == TokenType::STRING || toktype == TokenType::FORMAT_STRING
			|| toktype == TokenType::CHAR) {
		/* Save `\` literally when reading a CHAR & ignore unescaped `\` when reading a STRING */
		if (toktype == TokenType::CHAR
				|| (chr != TokenType::BACKSLASH || escaped(TokenType::BACKSLASH)))
			save(chr);

		if (((toktype == TokenType::STRING || toktype == TokenType::FORMAT_STRING)
				&& unescaped(TokenType::QUOTE))
				|| (toktype == TokenType::CHAR && unescaped(TokenType::SINGLE_QUOTE)))
			push();

		return;
	}

	/* Finalize current token if a punctuator is met */
	else if (Token::is_punctuator(chr)) {
		push();
		deduce_separator(*std::prev(cur_chr));
		begin(chr);
		return;
	}

	/* Save non-punctuator characters including `_` */
	if (std::isalnum(chr) || chr == TokenType::UNDERSCORE)
		save(chr);
}

void Lexer::deduce_separator(char chr)
{
	if (chr == '\n' || chr == Token::chr(TokenType::SEMICOLON))
		new_line();
	else if (cur_sep != Separator::NEWLINE && std::isspace(chr))
		cur_sep = Separator::SPACE;
}

void Lexer::deduce_separator()
{
	deduce_separator(*cur_chr);
}

/* Consume chrs one-by-one, accumulating them in a buffer & deducing the token type */
void Lexer::consume_and_deduce()
{
	if (toktype == TokenType::NONE)
		begin(*cur_chr);
	else
		consume();

	deduce_separator();
	next_char();
}

void Lexer::next_char()
{
	if (has_next()) {
		++cur_chr;
		++column;
	}
}

void Lexer::new_line()
{
	cur_sep = Separator::NEWLINE;
	column = 1;
	++line;
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
	cur_chr = code.begin();
	input_end = code.end();
	reset(!preserve_state);

	if constexpr (DEBUG)
		std::cout << "Lexing the input..." << std::endl;

	while (has_next())
		consume_and_deduce();
	push();

	if constexpr (DEBUG)
		std::cout << '\n' << "* Lexing complete. " << tokens.size() << " tokens parsed." << std::endl;

	if (!preserve_state)
		tokens.push(Token::EOF_TOKEN);
}

void Lexer::reset(bool full)
{
	column = 1;
	if (full) {
		line = 1;
		std::queue<Token>().swap(tokens);
		open_blocks = 0;
	}
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

Token& Lexer::last()
{
	return tokens.back();
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
