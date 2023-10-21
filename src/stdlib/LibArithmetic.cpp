#include "LibArithmetic.h"

#include <iterator>
#include <variant>
#include <type_traits>

#include <lexer/Token.h>
#include <util/array.h>
#include <util/meta/for_each.h>
#include <structure/Value.h>

namespace mtl
{

METHAN0L_LIBRARY(LibArithmetic)

void LibArithmetic::load()
{
	/* Binary operators */
	for_each({
		/* + - * / */
		Tokens::PLUS, Tokens::MINUS,
		Tokens::ASTERISK, Tokens::SLASH,

		/* += -= *= /= */
		Tokens::ADD, Tokens::SUB,
		Tokens::MUL, Tokens::DIV,

		/* % %= */
		Tokens::PERCENT, Tokens::COMP_MOD,

		/* & | ^ << >> */
		Tokens::BIT_AND, Tokens::BIT_OR, Tokens::BIT_XOR,
		Tokens::SHIFT_L, Tokens::SHIFT_R,

		/* &= |= ^= <<= >>= */
		Tokens::COMP_AND, Tokens::COMP_OR, Tokens::COMP_XOR,
		Tokens::COMP_SHIFT_L, Tokens::COMP_SHIFT_R
	}, [this](auto op) {
		infix_operator(op, bin_operator(op));
	});

	/* Unary minus */
	prefix_operator(Tokens::MINUS, UnaryOpr([this](Value &rhs) {
		return rhs.is<Int>() ?
			Value(-rhs.get<Int>()) : Value(-rhs.get<double>());
	}));

	/* Prefix & Postfix increment / decrement */
	for_each({Tokens::INCREMENT, Tokens::DECREMENT}, [this](auto op) {
		prefix_operator(op, UnaryOpr([=, this](auto &rhs) {
			apply_unary(rhs.get(), op);
			return rhs.get();
		}));

		postfix_operator(op, UnaryOpr([=, this](auto &lhs) {
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
			op == Tokens::INCREMENT ? ++n : --n;
		else
			throw InvalidTypeException(val.type());
	});
}

} /* namespace mtl */
