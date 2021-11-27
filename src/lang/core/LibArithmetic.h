#ifndef SRC_LANG_LIBARITHMETIC_H_
#define SRC_LANG_LIBARITHMETIC_H_

#include "../Library.h"

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
		Value calculate(ExprPtr &l, TokenType op, ExprPtr &r);
		template<typename T>
		T calculate(T l, TokenType op, T r)
		{
			switch (op) {
			case TokenType::ADD:
			case TokenType::PLUS:
				return l + r;

			case TokenType::SUB:
			case TokenType::MINUS:
				return l - r;

			case TokenType::MUL:
			case TokenType::ASTERISK:
				return l * r;

			case TokenType::DIV:
			case TokenType::SLASH:
				return l / r;

			default:
				return 0;
			}
		}

		void apply_unary(Value &val, TokenType op);

		inline int unary_diff(TokenType op)
		{
			return op == TokenType::INCREMENT ? 1 : -1;
		}
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBARITHMETIC_H_ */
