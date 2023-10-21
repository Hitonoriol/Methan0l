#ifndef SRC_LANG_LIBARITHMETIC_H_
#define SRC_LANG_LIBARITHMETIC_H_

#include <lexer/Token.h>
#include <core/Library.h>
#include <structure/Value.h>

namespace mtl
{

class LibArithmetic: public Library
{
	public:
		using Library::Library;
		void load() override;

	private:
		void apply_unary(Value &val, TokenType op);

		template<typename T>
		static BinaryOpr apply(TokenType op)
		{
			if (op == Tokens::ADD || op == Tokens::PLUS)
				return [](auto &l, auto &r) {return l.template as<T>() + r.template as<T>();};

			else if (op == Tokens::SUB || op == Tokens::MINUS)
				return [](auto &l, auto &r) {return l.template as<T>() - r.template as<T>();};

			else if (op == Tokens::MUL || op == Tokens::ASTERISK)
				return [](auto &l, auto &r) {return l.template as<T>() * r.template as<T>();};

			else if (op == Tokens::DIV || op == Tokens::SLASH)
				return [](auto &l, auto &r) {return l.template as<T>() / r.template as<T>();};

			IF (std::is_integral_v<T>) {
				if (op == Tokens::PERCENT || op == Tokens::COMP_MOD)
					return [](auto &l, auto &r) {return l.template as<T>() % r.template as<T>();};

				else if (op == Tokens::BIT_OR || op == Tokens::COMP_OR)
					return [](auto &l, auto &r) {return l.template as<T>() | r.template as<T>();};

				else if (op == Tokens::BIT_AND || op == Tokens::COMP_AND)
					return [](auto &l, auto &r) {return l.template as<T>() & r.template as<T>();};

				else if (op == Tokens::BIT_XOR || op == Tokens::COMP_XOR)
					return [](auto &l, auto &r) {return l.template as<T>() ^ r.template as<T>();};

				else if (op == Tokens::SHIFT_L || op == Tokens::COMP_SHIFT_L)
					return [](auto &l, auto &r) {return l.template as<T>() << r.template as<T>();};

				else if (op == Tokens::SHIFT_R || op == Tokens::COMP_SHIFT_R)
					return [](auto &l, auto &r) {return l.template as<T>() >> r.template as<T>();};
			}

			return [op](auto&, auto&) {
				throw std::runtime_error(
					"Arithmetic error: invalid operand type for operator `" + Token::to_string(op) + "`"
				);
				return Value::NO_VALUE;
			};
		}

		static BinaryOpr bin_operator(TokenType op)
		{
			auto dbl_op = apply<Float>(op);
			auto int_op = apply<Int>(op);

			return BinaryOpr([int_op, dbl_op](Value &lhs, Value &rhs) {
				lhs.get() = Value::is_double_op(lhs.get(), rhs.get()) ? dbl_op(lhs, rhs) : int_op(lhs, rhs);
				return lhs;
			});
		}
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBARITHMETIC_H_ */
