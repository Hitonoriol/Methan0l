#include <lang/core/library/LibArithmetic.h>
#include <lexer/Token.h>
#include <iterator>
#include <variant>
#include <type_traits>

#include "util/array.h"
#include "util/meta/for_each.h"
#include "structure/Value.h"
#include "type.h"

namespace mtl
{

METHAN0L_LIBRARY(LibArithmetic)

void LibArithmetic::load()
{
	/* Basic operators + compound assignment */
	Tokens<	/* + - * / */
			TokenType::PLUS, TokenType::MINUS,
			TokenType::ASTERISK, TokenType::SLASH,

			/* += -= *= /= */
			TokenType::ADD, TokenType::SUB,
			TokenType::MUL, TokenType::DIV,

			/* % %= */
			TokenType::PERCENT, TokenType::COMP_MOD,

			/* & | ^ << >> */
			TokenType::BIT_AND, TokenType::BIT_OR, TokenType::BIT_XOR,
			TokenType::SHIFT_L, TokenType::SHIFT_R,

			/* &= |= ^= <<= >>= */
			TokenType::COMP_AND, TokenType::COMP_OR, TokenType::COMP_XOR,
			TokenType::COMP_SHIFT_L, TokenType::COMP_SHIFT_R>
	::for_each([this](auto const &op) {
		infix_operator(op, bin_operator<op.value>());
	});

	/* Unary minus */
	prefix_operator(TokenType::MINUS, UnaryOpr([this](Value &rhs) {
		return rhs.is<Int>() ?
			Value(-rhs.get<Int>()) : Value(-rhs.get<double>());
	}));

	/* Prefix & Postfix increment / decrement */
	for_each({TokenType::INCREMENT, TokenType::DECREMENT}, [this](auto op) {
		prefix_operator(op, UnaryOpr([=](auto &rhs) {
			apply_unary(rhs.get(), op);
			return rhs.get();
		}));

		postfix_operator(op, UnaryOpr([=](auto &lhs) {
			Value tmp(lhs.get());
			apply_unary(lhs.get(), op);
			return tmp;
		}));
	});
}

void LibArithmetic::apply_unary(Value &val, TokenType op)
{
	val.accept([op, &val](auto &n) {
		if constexpr (std::is_arithmetic<VT(n)>::value && !std::is_same<VT(n), bool>::value)
			op == TokenType::INCREMENT ? ++n : --n;
		else
			throw InvalidTypeException(val.type());
	});
}

} /* namespace mtl */
