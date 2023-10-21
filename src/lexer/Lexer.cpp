#include "Lexer.h"

#include <bitset>

#include <util/util.h>

namespace mtl
{

const std::string Lexer::digits = "1234567890";
const std::string Lexer::double_digits = digits + '.';

const std::unordered_map<char, char> Lexer::escape_seqs = {
		{ 'a', '\a' },
		{ 'b', '\b' },
		{ 'f', '\f' },
		{ 'r', '\r' },
		{ 'n', '\n' },
		{ 'v', '\v' },
		{ 't', '\t' },
		{ '0', '\0' },
};

Lexer::Lexer() : tokens() {}

Lexer::Lexer(const Lexer &rhs) : tokens(rhs.tokens) {}

Lexer& Lexer::operator=(const Lexer &rhs)
{
	tokens = rhs.tokens;
	return *this;
}

void Lexer::register_punctuator(TokenType punctuator)
{
	punctuators.insert(punctuator);
}

void Lexer::register_keyword(TokenType keyword, bool hard)
{
	if (hard)
		hard_word_ops.push_back(keyword);
	else
		soft_word_ops.push_back(keyword);
}

void Lexer::register_operator(TokenType op)
{
	if (op->size() > 1)
		multichar_ops.push_back(op);
	else {
		single_char_ops.push_back(op);
		register_punctuator(op);
	}
}

void Lexer::register_block_begin_token(TokenType tok)
{
	block_begin_tokens.push_back(tok);
}

void Lexer::register_block_end_token(TokenType tok)
{
	block_end_tokens.push_back(tok);
}

UInt bin_to_int(const std::string &binstr)
{
	if (binstr.empty())
		return 0;

	UInt val = 0;
	for (char c : binstr) {
		val <<= 1;
		val += c - '0';
	}
	return val;
}

void Lexer::clear()
{
	tokstr.clear();
	toktype = Tokens::NONE;
}

void Lexer::save(char chr)
{
	tokstr += chr;
}

void Lexer::deduce_reserved()
{
	auto it = std::find(reserved_words.begin(), reserved_words.end(), tokstr);
	if (it == reserved_words.end())
		return;

	/* Boolean literal */
	else if (*it == ReservedWord::TRUE || *it == ReservedWord::FALSE)
		toktype = Tokens::BOOLEAN;
}

void Lexer::deduce_word_op()
{
	auto hard_op = std::find(hard_word_ops.begin(), hard_word_ops.end(), tokstr);
	auto soft_op = std::find(soft_word_ops.begin(), soft_word_ops.end(), tokstr);

	// Hard keyword - always lexed as its own type, can't be redefined by user
	if (hard_op != hard_word_ops.end()) {
		toktype = *hard_op;
		return;
	}

	if (soft_op != soft_word_ops.end() && match_next_punctuator(Tokens::COLON)) {
		toktype = *soft_op;
		return;
	}

}

void Lexer::push()
{
	if (tokstr.empty())
		return;

	if (is_block_begin(toktype))
		++open_blocks;

	else if (toktype == Tokens::IDENTIFIER) {
		deduce_reserved();
		deduce_word_op();
	}

	else if (toktype == Tokens::CHAR) {
		strip_quotes(tokstr);
	}

	else if (toktype == Tokens::INTEGER && cur_int_literal != IntLiteral::DEC) {
		UInt val;
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
	if (chr != Tokens::SEMICOLON && chr != Tokens::NEWLINE) {
		auto ctype = to_tok(chr);
		tokens.push(finalize_token(Token(ctype)));

		if (is_block_begin(ctype))
			++open_blocks;
		else if (is_block_end(ctype))
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
	tok.set_line(cur_sep == Separator::NEWLINE ? line - 1 : line);
	tok.set_column(column);
	tok.set_separator(cur_sep);
	cur_sep = Separator::NONE;
	/* If this token is a token literal (e.g. `token`) */
	if (token_literal) {
		tok.set_type(Tokens::TOKEN);
		next_char(); // Skip the second '`'
		token_literal = false;
	}
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
	for (auto it = std::next(cur_chr, 2); it != input_end && is_punctuator(*it); ++it)
		op += *it;

	/* Test from longest to shortest possible punctuator combination */
	TokenType multichar_type = Tokens::NONE;
	while(op.length() >= 2) {
		if ((multichar_type = get_multichar_op_type(op)) != Tokens::NONE)
			break;
		op.erase(op.length() - 1);
	}

	if (multichar_type == Tokens::NONE)
		return false;

	/* Skip everything inside a block comment */
	if (multichar_type == Tokens::BLOCK_COMMENT_L) {
		do {
			next_char();
		} while (*cur_chr != Tokens::ASTERISK.chr() ||
				*std::next(cur_chr) != Tokens::SLASH.chr());
		next_char();
		clear();
		return true;
	}

	tokstr = op;
	toktype = multichar_type;
	next_char(tokstr.size() - 1); // First token was already consumed by this point
	push();
	return true;
}

/* Assume token type by the first character */
void Lexer::begin(char chr)
{
	cur_int_literal = IntLiteral::NONE;
	/* Formatted String literal: $"..." */
	if (chr == Tokens::LIST && match_next(Tokens::QUOTE)) {
		toktype = Tokens::FORMAT_STRING;
		next_char();
		begin(*cur_chr);
	}

	/* String / Char literal */
	else if (chr == Tokens::QUOTE || chr == Tokens::SINGLE_QUOTE) {
		if (toktype != Tokens::FORMAT_STRING)
			toktype = (chr == Tokens::QUOTE) ? Tokens::STRING : Tokens::CHAR;
		save(chr);
	}

	/* Token literal */
	else if (chr == Tokens::QUOTE_ALT) {
		token_literal = true;
	}

	/* Punctuator / Compound punctuator token */
	else if (!std::isalnum(chr) && is_punctuator(chr)) {
		char next = look_ahead();
		if (is_punctuator(next))
			if (try_save_multichar_op(chr, next))
				return;

		push(chr);
		clear();
		return;
	}

	else if (std::isdigit(chr)) {
		toktype = Tokens::INTEGER;
		save(chr);
	}

	else if (std::isalpha(chr) || chr == Tokens::UNDERSCORE) {
		toktype = Tokens::IDENTIFIER;
		save(chr);
	}
}

/* Continue consuming after the starting assumption */
void Lexer::consume()
{
	char chr = *cur_chr;

	/* Ignore underscores when saving Number literals
	 * (this allows to separate digits like so: `123_456_789.912_345`)
	 */
	if (chr == Tokens::UNDERSCORE.chr()) {
		if (saving_number())
			return;
	}

	/* Deduce floating point literal */
	if (toktype == Tokens::INTEGER && chr == Tokens::DOT) {
		if (!std::isdigit(*std::next(cur_chr))) {
			push();
			begin(chr);
			return;
		}

		save(chr);
		toktype = Tokens::DOUBLE;
		return;
	}

	/* Deduce integral literal type */
	else if (toktype == Tokens::INTEGER && cur_int_literal == IntLiteral::NONE) {
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

	else if (saving_number() && is_separator(chr)) {
		push();
		begin(chr);
		return;
	}

	/* Save String / Character literal char */
	else if (saving_string()) {
		/* Save `\` literally when reading a CHAR & ignore unescaped `\` when reading a STRING */
		if (chr != Tokens::BACKSLASH || escaped(Tokens::BACKSLASH)) {
			if (match_prev(Tokens::BACKSLASH) && *std::prev(cur_chr, 2) != Tokens::BACKSLASH
					&& std::isalpha(chr))
				save(escape_seq(chr));
			else
				save(chr);
		}

		if (((toktype == Tokens::STRING || toktype == Tokens::FORMAT_STRING)
				&& unescaped(Tokens::QUOTE))
				|| (toktype == Tokens::CHAR && unescaped(Tokens::SINGLE_QUOTE)))
			push();

		return;
	}

	/* Space acts as a token separator for all tokens except Number & String-like literals */
	else if (std::isspace(chr)) {
		push();
		return;
	}

	/* Finalize current token if a punctuator is met */
	else if (is_punctuator(chr)) {
		push();
		deduce_separator(*std::prev(cur_chr));
		begin(chr);
		return;
	}

	/* Save non-punctuator characters including `_` */
	if (std::isalnum(chr) || chr == Tokens::UNDERSCORE)
		save(chr);
}

void Lexer::deduce_separator(char chr)
{
	if (chr == NL || chr == Tokens::SEMICOLON.chr())
		cur_sep = Separator::NEWLINE;
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
	if (toktype == Tokens::NONE)
		begin(*cur_chr);
	else
		consume();

	deduce_separator();
	next_char();
}

void Lexer::next_char(size_t n)
{
	if (!has_next())
		return;

	for (size_t i = 0; i < n; ++i) {
		++cur_chr;
		++column;
		if (*cur_chr == NL)
			new_line();
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
	return *cur_chr == tok.chr();
}

inline bool Lexer::match_prev(TokenType tok)
{
	return *std::prev(cur_chr) == tok;
}

inline bool Lexer::match_next(TokenType tok)
{
	return *std::next(cur_chr) == tok;
}

bool Lexer::match_next_punctuator(TokenType tok)
{
	for (auto it = cur_chr; it != input_end; ++it) {
		if (*it == tok)
			return true;
		if (is_punctuator(*it))
			return false;
	}
	return false;
}

inline bool Lexer::unescaped(TokenType tok)
{
	return match_cur(tok) && !match_prev(Tokens::BACKSLASH);
}

inline bool Lexer::escaped(TokenType tok)
{
	return match_cur(tok) && match_prev(Tokens::BACKSLASH);
}

void Lexer::lex(std::string &code, bool preserve_state)
{
	cur_chr = code.begin();
	input_end = code.end();
	reset(!preserve_state);

	if constexpr (DEBUG)
		std::cout << "Lexing the input..." << std::endl;

	if (*cur_chr == NL)
		new_line();

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

TokenType Lexer::get_multichar_op_type(std::string_view op)
{
	auto it = std::find(multichar_ops.begin(), multichar_ops.end(), op);
	if (it != multichar_ops.end())
		return *it;
	return Tokens::NONE;
}

bool Lexer::is_hard_keyword(std::string_view typestr)
{
	return std::find(hard_word_ops.begin(), hard_word_ops.end(), typestr) != hard_word_ops.end();
}

bool Lexer::is_soft_keyword(std::string_view typestr)
{
	return std::find(soft_word_ops.begin(), soft_word_ops.end(), typestr) != soft_word_ops.end();
}

bool Lexer::is_keyword(std::string_view typestr)
{
	return is_hard_keyword(typestr) || is_soft_keyword(typestr);
}

bool Lexer::is_block_begin(TokenType tok)
{
	return std::find(block_begin_tokens.begin(), block_begin_tokens.end(), tok) != block_begin_tokens.end();
}

bool Lexer::is_block_end(TokenType tok)
{
	return std::find(block_end_tokens.begin(), block_end_tokens.end(), tok) != block_end_tokens.end();
}

bool Lexer::is_blank(char chr)
{
	return std::isspace(chr) || chr == NL;
}

bool Lexer::is_punctuator(char chr)
{
	return contains(punctuators, chr);
}

bool Lexer::is_separator(char chr)
{
	return is_blank(chr) || is_punctuator(chr);
}

char Lexer::escape_seq(char seq)
{
	return escape_seqs.at(seq);
}

char Lexer::to_chr(TokenType tok)
{
	return tok.name[0];
}

TokenType Lexer::to_tok(char chr)
{
	auto it = std::find(punctuators.begin(), punctuators.end(), chr);
	if (it != punctuators.end())
		return *it;
	return Tokens::NONE;
}

bool Lexer::is_semantic(const TokenType &tok)
{
	return contains(semantic_tokens, tok);
}

bool Lexer::is_transparent(const TokenType &tok)
{
	return contains(transparent_tokens, tok);
}

} /* namespace mtl */
