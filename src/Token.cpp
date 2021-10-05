#include "Token.h"

#include <iostream>
#include <sstream>

#include "expression/IdentifierExpr.h"
#include "methan0l_type.h"
#include "util.h"

namespace mtl
{

const Token Token::END_OF_EXPR(TokenType::EXPR_END);
const Token Token::EOF_TOKEN(TokenType::END, "end of program");
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

Token& Token::operator=(const Token &rhs)
{
	value = rhs.value;
	type = rhs.type;
	return *this;
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

bool Token::is_delimiter(char chr)
{
	switch (static_cast<TokenType>(chr)) {
	case TokenType::SEMICOLON:
		case TokenType::NEWLINE:
		return true;

	default:
		return false;
	}
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

bool Token::is_reserved(std::string &tokstr)
{
	auto end = std::end(reserved_words);
	return std::find(std::begin(reserved_words), end, tokstr) != end;
}

TokenType Token::as_word_op(std::string &tokstr)
{
	auto begin = std::begin(word_ops), end = std::end(word_ops);
	auto word = std::find(begin, end, tokstr);
	return word != end ?
			static_cast<TokenType>(WORD_OP_START + std::distance(begin, word)) :
			TokenType::NONE;
}

std::string_view Token::word_op(TokenType tok)
{
	return word_ops[static_cast<int>(tok) - WORD_OP_START];
}

std::string_view Token::bichar_op(TokenType tok)
{
	return bichar_ops[static_cast<int>(tok) - BICHAR_OP_START];
}

bool Token::is_compatible(ExprPtr expr, TokenType next)
{
	switch (next) {
	/* Because ++ and -- are both prefix & postfix, additional check is needed to avoid syntax "collisions" */
	case TokenType::INCREMENT:
		case TokenType::DECREMENT:
		return instanceof<IdentifierExpr>(expr.get());

	default:
		return true;
	}
}

char Token::chr(TokenType tok)
{
	if (static_cast<int>(tok) < BICHAR_OP_START)
		return static_cast<char>(tok);

	return '\0';
}

bool Token::contains_all(std::string str, std::string substr)
{
	return str.find_first_not_of(substr) == std::string::npos;
}

std::string Token::to_string()
{
	std::ostringstream ss;
	ss << *this;
	return ss.str();
}

std::ostream& operator <<(std::ostream &stream, const Token &val)
{
	stream << "{\"" << val.value << "\" : ";
	const int type = static_cast<int>(val.type);

	if (type < 0x7F)
		stream << static_cast<char>(val.type);

	else if (type < Token::LITERAL_START)
		stream << Token::bichar_op(val.type);

	else if (type < Token::WORD_OP_START)
		stream << "Literal Value / Identifier";

	else if (type < Token::MISC_TOKENS_START)
		stream << Token::word_op(val.type);

	else
		stream << "[Internal Interpreter Token]";

	return stream << "}";
}

}
/* namespace mtl */
