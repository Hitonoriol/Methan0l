#ifndef SRC_LANG_LIBARITHMETIC_H_
#define SRC_LANG_LIBARITHMETIC_H_

#include "../Library.h"
#include "Token.h"
#include "structure/Value.h"

#define IF(condition) if constexpr (condition)
#define ELIF(condition) else IF(condition)

namespace mtl
{

class LibArithmetic: public Library
{
	public:
		LibArithmetic(ExprEvaluator *eval) : Library(eval)
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
				IF (op == TokenType::BIT_OR)
					return l | r;

				ELIF (op == TokenType::BIT_AND)
					return l & r;

				ELIF (op == TokenType::BIT_XOR)
					return l ^ r;

				ELIF (op == TokenType::SHIFT_L)
					return l << r;

				ELIF (op == TokenType::SHIFT_R)
					return l >> r;
			}

			throw std::runtime_error("Arithmetic error: invalid operand type");
		}

		template <TokenType op>
		static constexpr BinaryOpr bin_operator()
		{
			return BinaryOpr([](Value &lhs, Value &rhs) {
				if (Value::is_double_op(lhs.get(), rhs))
					return lhs.get() = apply<op>(lhs.as<double>(), rhs.as<double>());

				return lhs.get() = apply<op>(lhs.as<dec>(), rhs.as<dec>());
			});
		}
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBARITHMETIC_H_ */
