#include "Lexer.h"

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
	if (reserved == Word::TRUE || reserved == Word::FALSE)
		toktype = TokenType::BOOLEAN;

}

void Lexer::push()
{
	if (tokstr.empty())
		return;

	if (toktype == TokenType::IDENTIFIER)
		deduce_reserved();

	if constexpr (DEBUG)
		std::cout << "[" << tokstr << "] ";

	tokens.push(Token(toktype, tokstr));
	clear();
}

void Lexer::push(char chr)
{
	if (chr != TokenType::SEMICOLON && chr != TokenType::NEWLINE) {
		tokens.push(Token(chr));
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
	if (chr == TokenType::QUOTE) {
		toktype = TokenType::STRING;
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
void Lexer::consume(char chr)
{
	if (toktype == TokenType::INTEGER && chr == TokenType::DOT) {
		save(chr);
		toktype = TokenType::DOUBLE;
		return;
	}

	else if (toktype == TokenType::STRING) {
		save(chr);
		if (chr == TokenType::QUOTE)
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

/* Consume all characters of the provided code */
void Lexer::consume()
{
	if (toktype == TokenType::NONE)
		begin(*cur_chr);
	else
		consume(*cur_chr);

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

void Lexer::parse(std::string code)
{
	cur_chr = code.begin();
	input_end = code.end();
	std::queue<Token>().swap(tokens);

	while (has_next())
		consume();
	tokens.push(Token::EOF_TOKEN);
}

Token Lexer::next()
{
	if (tokens.empty())
		return Token::EOF_TOKEN;

	Token token = tokens.front();
	tokens.pop();
	return token;
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
