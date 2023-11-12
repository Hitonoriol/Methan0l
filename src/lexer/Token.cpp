#include "Token.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include <type.h>
#include <util/util.h>
#include <util/array.h>
#include <expression/BinaryOperatorExpr.h>
#include <expression/IdentifierExpr.h>
#include <expression/UnitExpr.h>
#include <expression/FunctionExpr.h>
#include <expression/InvokeExpr.h>

namespace mtl
{

const Token Token::END_OF_EXPR(Tokens::EXPR_END);
const Token Token::EOF_TOKEN(Tokens::END, "end of program");

Token::Token(TokenType type, std::string value) : type(type), value(value)
{
}

/* Empty token */
Token::Token() : Token(Tokens::NONE, "")
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
	return type == rhs.type;
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

bool Token::is_ref_opr(TokenType opr)
{
	switch (opr.id) {
		case Tokens::INCREMENT.id:
		case Tokens::DECREMENT.id:
		case Tokens::DIV.id:
		case Tokens::MUL.id:
		case Tokens::SUB.id:
		case Tokens::ADD.id:
		case Tokens::COMP_AND.id:
		case Tokens::COMP_OR.id:
		case Tokens::COMP_XOR.id:
		case Tokens::COMP_SHIFT_L.id:
		case Tokens::COMP_SHIFT_R.id:
		case Tokens::COMP_MOD.id:
		return true;

	default:
		return false;
	}
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
	return std::string(tok.name);
}

std::ostream& operator <<(std::ostream &stream, const Token &val)
{
	if (val.line != 0)
		stream << "[@" << val.line << ":" << val.column << "] ";

	stream << val.type.name;

	if (val.type.name != val.value && !val.value.empty())
		stream << " \"" << val.value << "\"";

	return stream;
}

}
/* namespace mtl */
