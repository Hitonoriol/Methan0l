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
		dec eval_bitwise(dec l, TokenType op, dec r);
		bool eval_logical(const ExprPtr &l, TokenType op, const ExprPtr &r);
		bool eval_arithmetic_comparison(double l, TokenType op, double r);
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBLOGICAL_H_ */
