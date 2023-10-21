#include <lexer/TokenType.h>

namespace mtl
{

std::string_view* TokenType::operator->()
{
	return &name;
}

char TokenType::chr() const
{
	return name[0];
}

} /* namespace mtl */
