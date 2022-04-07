#include "LibArithmetic.h"

#include <iterator>
#include <variant>
#include <type_traits>

#include "util/array.h"
#include "structure/Value.h"
#include "type.h"
#include "Token.h"

namespace mtl
{

void LibArithmetic::load()
{
	/* Basic arithmetic operators + Compound assignment */
	for_each({
			TokenType::PLUS, TokenType::MINUS,
			TokenType::ASTERISK, TokenType::SLASH,
			TokenType::ADD,
			TokenType::SUB, TokenType::MUL,
			TokenType::DIV
	},
	[this](auto op) {
		infix_operator(op, BinaryOpr([=](auto &lhs, auto &rhs) {
			Value res = calculate(lhs, op, rhs);
			return lhs = res;
		}));
	});

	/* Modulo operator */
	infix_operator(TokenType::PERCENT, BinaryOpr([&](auto &lhs, auto &rhs) {
		return mtl::num(lhs) % mtl::num(rhs);
	}));

	/* Unary minus */
	prefix_operator(TokenType::MINUS, UnaryOpr([this](Value &rhs) {
		return rhs.type() == Type::INTEGER ? -rhs.get<dec>() : -rhs.get<double>();
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

Value LibArithmetic::calculate(const Value &lhs, TokenType op, const Value &rhs)
{
	if (Value::is_double_op(lhs, rhs))
		return calculate(lhs.as<double>(), op, rhs.as<double>());

	return calculate(lhs.as<dec>(), op, rhs.as<dec>());
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
