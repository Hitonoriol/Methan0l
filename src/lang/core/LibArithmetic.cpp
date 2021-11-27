#include "LibArithmetic.h"

#include <iterator>
#include <variant>

#include "structure/Value.h"
#include "type.h"
#include "Token.h"

namespace mtl
{

void LibArithmetic::load()
{
	/* Arithmetic operators w\ implicit conversions INTEGER <--> DOUBLE */
	TokenType ar_ops[] = {
			TokenType::PLUS, TokenType::MINUS,
			TokenType::ASTERISK, TokenType::SLASH,
			TokenType::ADD,
			TokenType::SUB, TokenType::MUL,
			TokenType::DIV
	};
	for (size_t i = 0; i < std::size(ar_ops); ++i)
		infix_operator(ar_ops[i], [=](auto lhs, auto rhs) {
			Value res = calculate(lhs, ar_ops[i], rhs);

			if (Token::is_ref_opr(ar_ops[i]))
				ref(lhs) = res;

			return res;
		});

	infix_operator(TokenType::PERCENT, [&](ExprPtr lhs, ExprPtr rhs) {
		return Value(mtl::num(val(lhs)) % mtl::num(val(rhs)));
	});

	prefix_operator(TokenType::MINUS, [this](auto rhs) {
		Value rval = val(rhs);

		if (rval.type() == Type::INTEGER)
			rval = -rval.as<dec>();
		else
			rval = -rval.as<double>();

		return rval;
	});

	/* Prefix & Postfix increment / decrement */
	TokenType unary_ar_ops[] = { TokenType::INCREMENT, TokenType::DECREMENT };
	for (size_t i = 0; i < std::size(unary_ar_ops); ++i) {
		prefix_operator(unary_ar_ops[i], [=](auto rhs) {
			Value &val = ref(rhs);
			apply_unary(val, unary_ar_ops[i]);
			return val;
		});

		postfix_operator(unary_ar_ops[i], [=](auto lhs) {
			Value &val = ref(lhs);
			Value tmp(val);
			apply_unary(val, unary_ar_ops[i]);
			return tmp;
		});
	}
}

Value LibArithmetic::calculate(ExprPtr &l, TokenType op, ExprPtr &r)
{
	Value lexpr = val(l), rexpr = val(r);

	if (Value::is_double_op(lexpr, rexpr))
		return Value(calculate(lexpr.as<double>(), op, rexpr.as<double>()));

	return Value(calculate(lexpr.as<dec>(), op, rexpr.as<dec>()));
}

void LibArithmetic::apply_unary(Value &val, TokenType op)
{
	const int d = unary_diff(op);
	switch (val.type()) {
	case Type::INTEGER:
		val = val.as<dec>() + d;
		break;

	case Type::DOUBLE:
		val = val.as<double>() + d;
		break;

	default:
		break;
	}
}

} /* namespace mtl */
