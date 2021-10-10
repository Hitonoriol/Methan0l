#ifndef SRC_LANG_LIBLOGICAL_H_
#define SRC_LANG_LIBLOGICAL_H_

#include "../Library.h"

namespace mtl
{

class LibLogical: public Library
{
	public:
		LibLogical(ExprEvaluator *eval) : Library(eval) {}
		void load() override;

	private:
		int eval_bitwise(int l, TokenType op, int r);
		bool eval_logical(bool l, TokenType op, bool r);
		bool eval_logic_arithmetic(double l, TokenType op, double r);
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBLOGICAL_H_ */
