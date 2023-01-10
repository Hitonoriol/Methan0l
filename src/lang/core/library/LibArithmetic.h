#ifndef SRC_LANG_LIBARITHMETIC_H_
#define SRC_LANG_LIBARITHMETIC_H_

#include <lexer/Token.h>
#include <lang/Library.h>
#include "structure/Value.h"

namespace mtl
{

class LibArithmetic: public Library
{
	public:
		LibArithmetic(Interpreter *eval) : Library(eval)
		{
		}

		void load() override;

	private:
		void apply_unary(Value &val, TokenType op);

		template<TokenType op, typename T>
		static T apply(T l, T r)
		{
			IF (op == TokenType::ADD || op == TokenType::PLUS)
				return l + r;

			ELIF (op == TokenType::SUB || op == TokenType::MINUS)
				return l - r;

			ELIF (op == TokenType::MUL || op == TokenType::ASTERISK)
				return l * r;

			ELIF (op == TokenType::DIV || op == TokenType::SLASH)
				return l / r;

			ELIF (std::is_integral<VT(l)>::value && std::is_integral<VT(r)>::value) {
				IF (op == TokenType::PERCENT || op == TokenType::COMP_MOD)
					return l % r;

				ELIF (op == TokenType::BIT_OR || op == TokenType::COMP_OR)
					return l | r;

				ELIF (op == TokenType::BIT_AND || op == TokenType::COMP_AND)
					return l & r;

				ELIF (op == TokenType::BIT_XOR || op == TokenType::COMP_XOR)
					return l ^ r;

				ELIF (op == TokenType::SHIFT_L || op == TokenType::COMP_SHIFT_L)
					return l << r;

				ELIF (op == TokenType::SHIFT_R || op == TokenType::COMP_SHIFT_R)
					return l >> r;
			}

			throw std::runtime_error("Arithmetic error: invalid operand type ("
					+ mtl::str(l) + " " + Token::to_string(op) + " " + mtl::str(r) + ")");
		}

		template<TokenType op>
		static constexpr BinaryOpr bin_operator()
		{
			return BinaryOpr([](Value &lhs, Value &rhs) {
				if (Value::is_double_op(lhs.get(), rhs.get()))
					lhs.get() = apply<op>(lhs.as<double>(), rhs.as<double>());
				else
					lhs.get() = apply<op>(lhs.as<dec>(), rhs.as<dec>());
				return lhs;
			});
		}
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBARITHMETIC_H_ */
