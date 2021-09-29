#include "Token.h"

#include <iostream>

namespace mtl
{

const Token Token::EOF_TOKEN(TokenType::END, "");
const std::string Token::digits = "1234567890";
const std::string Token::double_digits = digits + '.';

Token::Token(TokenType type, std::string value) : type(type), value(value)
{
}

/* Empty token */
Token::Token() : Token(TokenType::NONE, "")
{
}

/* Punctuator token */
Token::Token(char chr) : Token(static_cast<TokenType>(chr), std::string(1, chr))
{
}

/* Misc token -- double char opr / reserved word / ... */
Token::Token(std::string op) : Token(deduce_type(op), op)
{
}

std::string_view Token::reserved(Word word)
{
	return reserved_words[static_cast<int>(word)];
}

bool Token::operator ==(const Token &rhs)
{
	return static_cast<int>(type) == static_cast<int>(rhs.type);
}

bool Token::operator !=(const Token &rhs)
{
	return !(*this == rhs);
}

bool operator ==(const char chr, const TokenType &token)
{
	return static_cast<char>(token) == chr;
}

bool operator !=(const char chr, const TokenType &token)
{
	return !(chr == token);
}

TokenType Token::get_type() const
{
	return type;
}

std::string& Token::get_value()
{
	return value;
}

TokenType Token::get_bichar_op_type(std::string &tokstr)
{
	for (size_t i = 0; i < std::size(bichar_ops); ++i)
		if (tokstr == bichar_ops[i])
			return static_cast<TokenType>(BICHAR_OP_START + i);

	return TokenType::NONE;
}

TokenType Token::deduce_type(std::string &tokstr)
{
	const size_t len = tokstr.size();
	if (len == 2)
		return get_bichar_op_type(tokstr);

	return TokenType::NONE;
}

bool Token::is_punctuator(char chr)
{
	for (size_t i = 0; i < std::size(punctuators); ++i)
		if (chr == punctuators[i])
			return true;
	return false;
}

Word Token::as_reserved(std::string &tokstr)
{
	for (size_t i = 0; i < std::size(reserved_words); ++i)
		if (tokstr == reserved_words[i])
			return static_cast<Word>(i);
	return Word::NIL;
}

char Token::chr(TokenType tok)
{
	return static_cast<char>(tok);
}

bool Token::contains_all(std::string str, std::string substr)
{
	return str.find_first_not_of(substr) == std::string::npos;
}

std::ostream& operator <<(std::ostream &stream, const Token &val)
{
	return stream << "{\"" << val.value << "\":" << static_cast<int>(val.type) << "}";
}

}
/* namespace mtl */
