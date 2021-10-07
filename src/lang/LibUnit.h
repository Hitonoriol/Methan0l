#ifndef SRC_LANG_LIBUNIT_H_
#define SRC_LANG_LIBUNIT_H_

#include "Library.h"

namespace mtl
{

class LibUnit: public Library
{
	public:
		LibUnit(ExprEvaluator *eval) : Library(eval) {}
		void load() override;

	private:
		Value invoke_method(ExprPtr obj, ExprPtr func);
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBUNIT_H_ */
