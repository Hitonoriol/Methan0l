#include "LibLogical.h"

#include <iterator>

#include <lexer/Token.h>
#include <structure/Value.h>
#include <util/array.h>
#include <core/Logic.h>

namespace mtl
{

METHAN0L_LIBRARY(LibLogical)

void LibLogical::load()
{
	/* Lazy logical operators: &&, ||, ^^ */
	for_each({Tokens::AND, Tokens::OR, Tokens::XOR}, [this](auto op) {
		infix_operator(op, LazyBinaryOpr([=, this](auto lhs, auto rhs) {
			return Value(core::logical_operation(*context, lhs, op, rhs));
		}));
	});

	/* Logical NOT operator */
	prefix_operator(Tokens::EXCLAMATION, UnaryOpr([this](Value &rhs) {
		return !rhs.as<bool>();
	}));

	/* Bitwise NOT */
	prefix_operator(Tokens::TILDE, LazyUnaryOpr([this](auto rhs) {
		return Value(~mtl::num(val(rhs)));
	}));

	/* Comparison operators: >, <, >=, <= */
	TokenType cmp_ops[] = {
			Tokens::GREATER, Tokens::GREATER_OR_EQ,
			Tokens::LESS, Tokens::LESS_OR_EQ
	};
	for (size_t i = 0; i < std::size(cmp_ops); ++i)
		infix_operator(cmp_ops[i], LazyBinaryOpr([&, opr = cmp_ops[i]](auto lhs, auto rhs) {
			Value lval = val(lhs), rval = val(rhs);
			return Value(core::arithmetic_comparison(lval.as<double>(), opr, rval.as<double>()));
		}));

	/* Equals operator */
	infix_operator(Tokens::EQUALS, LazyBinaryOpr([&](auto lhs, auto rhs) {
		return Value(val(lhs) == val(rhs));
	}));

	infix_operator(Tokens::NOT_EQUALS, LazyBinaryOpr([&](auto lhs, auto rhs) {
		return Value(val(lhs) != val(rhs));
	}));
}

} /* namespace mtl */
