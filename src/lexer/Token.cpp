#include <lexer/Token.h>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "type.h"
#include "util/util.h"
#include "util/array.h"

#include "expression/BinaryOperatorExpr.h"
#include "expression/IdentifierExpr.h"
#include "expression/UnitExpr.h"
#include "expression/FunctionExpr.h"
#include "expression/InvokeExpr.h"

namespace mtl
{

const Token Token::END_OF_EXPR(TokenType::EXPR_END);
const Token Token::EOF_TOKEN(TokenType::END, "end of program");
const std::string Token::digits = "1234567890";
const std::string Token::double_digits = digits + '.';
const int Token::TYPENAMES_BEG_IDX =
		std::distance(std::begin(reserved_words),
				std::find(std::begin(reserved_words), std::end(reserved_words), Token::reserved(Word::T_NIL)));

const std::unordered_map<char, char> Token::escape_seqs = {
		{ 'a', '\a' },
		{ 'b', '\b' },
		{ 'f', '\f' },
		{ 'r', '\r' },
		{ 'n', '\n' },
		{ 'v', '\v' },
		{ 't', '\t' },
		{ '0', '\0' },
};

Token::Token(TokenType type, std::string value) : type(type), value(value)
{
}

/* Empty token */
Token::Token() : Token(TokenType::NONE, "")
{
}

Token::Token(const Token &rhs)
{
	operator =(rhs);
}

Token::Token(Token &&rhs)
{
	value = std::move(rhs.value);
	type = rhs.type;
	line = rhs.line;
	column = rhs.column;
	sep = rhs.sep;
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
	line = rhs.line;
	column = rhs.column;
	sep = rhs.sep;
	return *this;
}

void Token::assert_type(TokenType type)
{
	if (this->type != type)
		throw std::runtime_error("Token type assertion on " + to_string() + " failed. "
				"Expected: " + to_string(type));
}

bool Token::is_identical(const Token &rhs) const
{
	return operator ==(rhs) && line == rhs.line && column == rhs.column;
}

bool Token::operator ==(const Token &rhs) const
{
	return static_cast<int>(type) == static_cast<int>(rhs.type);
}

bool Token::operator !=(const Token &rhs) const
{
	return !(*this == rhs);
}

bool Token::operator ==(const TokenType &rhs)
{
	return type == rhs;
}

bool Token::operator !=(const TokenType &rhs)
{
	return type != rhs;
}

bool operator ==(const char chr, const TokenType &token)
{
	return static_cast<char>(token) == chr;
}

bool operator !=(const char chr, const TokenType &token)
{
	return !(chr == token);
}

void Token::set_type(TokenType type)
{
	this->type = type;
}

TokenType Token::get_type() const
{
	return type;
}

const std::string& Token::get_value() const
{
	return value;
}

void Token::set_line(uint32_t line)
{
	this->line = line;
}

uint32_t Token::get_line() const
{
	return line;
}

void Token::set_column(uint32_t col)
{
	column = col;
}

void Token::set_separator(Separator sep)
{
	this->sep = sep;
}

uint32_t Token::get_column() const
{
	return column;
}

TokenType Token::get_multichar_op_type(std::string &tokstr)
{
	for (size_t i = 0; i < std::size(multichar_ops); ++i)
		if (tokstr == multichar_ops[i])
			return static_cast<TokenType>(BICHAR_OP_START + i);

	return TokenType::NONE;
}

TokenType Token::deduce_type(std::string &tokstr)
{
	const size_t len = tokstr.size();
	if (len == 2)
		return get_multichar_op_type(tokstr);

	return TokenType::NONE;
}

char Token::escape_seq(char seq)
{
	return escape_seqs.at(seq);
}

bool Token::is_punctuator(char chr)
{
	for (size_t i = 0; i < std::size(punctuators); ++i)
		if (chr == punctuators[i])
			return true;
	return false;
}

bool Token::is_keyword(TokenType tok)
{
	return contains(keywords, tok);
}

Word Token::as_reserved(std::string &tokstr)
{
	for (size_t i = 0; i < std::size(reserved_words); ++i)
		if (tokstr == reserved_words[i])
			return static_cast<Word>(i);
	return Word::NIL;
}

bool Token::is_block_begin(TokenType tok)
{
	return contains(block_begin_tokens, tok);
}

bool Token::is_block_end(char c)
{
	return contains(block_end_tokens, tok(c));
}

bool Token::is_semantic(const TokenType &tok)
{
	return contains(semantic_tokens, tok);
}

bool Token::is_transparent(const TokenType &tok)
{
	return contains(transparent_tokens, tok);
}

bool Token::is_reserved(std::string &tokstr)
{
	return contains(reserved_words, tokstr);
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
	return multichar_ops[static_cast<int>(tok) - BICHAR_OP_START];
}

bool Token::is_ref_opr(TokenType opr)
{
	switch (opr) {
		case TokenType::INCREMENT:
		case TokenType::DECREMENT:
		case TokenType::DIV:
		case TokenType::MUL:
		case TokenType::SUB:
		case TokenType::ADD:
		case TokenType::COMP_AND:
		case TokenType::COMP_OR:
		case TokenType::COMP_XOR:
		case TokenType::COMP_SHIFT_L:
		case TokenType::COMP_SHIFT_R:
		case TokenType::COMP_MOD:
		return true;

	default:
		return false;
	}
}

char Token::chr(TokenType tok)
{
	if (static_cast<int>(tok) < BICHAR_OP_START)
		return static_cast<char>(tok);

	return '\0';
}

TokenType Token::tok(char chr)
{
	if ((int) chr < BICHAR_OP_START)
		return static_cast<TokenType>(chr);

	return TokenType::NONE;
}

bool Token::contains_all(std::string str, std::string substr)
{
	return str.find_first_not_of(substr) == std::string::npos;
}

std::string Token::to_string() const
{
	std::ostringstream ss;
	ss << *this;
	return ss.str();
}

std::string Token::to_string(TokenType tok)
{
	const int type = static_cast<int>(tok);

	if (type < PRINTABLE_CHARS_END)
		return str(static_cast<char>(tok));

	else if (type < Token::LITERAL_START)
		return std::string(Token::bichar_op(tok));

	else if (type < Token::WORD_OP_START)
		return "[Literal Value / Identifier]";

	else if (type < Token::MISC_TOKENS_START)
		return std::string(Token::word_op(tok));

	else
		return "[Internal Token]";
}

std::ostream& operator <<(std::ostream &stream, const Token &val)
{
	bool printable = static_cast<int>(val.type) < Token::MISC_TOKENS_START;
	stream << "[";

	if (val.line != 0)
		stream << "(@" << val.line << ":" << val.column << ") ";

	if (!printable)
		stream << "`" << val.value << "` : ";
	else
		stream << "`";

	stream << Token::to_string(val.type);
	if (printable)
		stream << "`";

	return stream << "]";
}

}
/* namespace mtl */