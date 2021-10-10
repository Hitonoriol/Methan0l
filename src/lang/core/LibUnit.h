#ifndef SRC_LANG_LIBUNIT_H_
#define SRC_LANG_LIBUNIT_H_

#include "../../type.h"
#include "../Library.h"

namespace mtl
{

class LibUnit: public Library
{
	public:
		LibUnit(ExprEvaluator *eval) : Library(eval) {}
		void load() override;

	private:
		Value object_dot_operator(Object &obj, ExprPtr rhs);
		Value invoke_pseudo_method(ExprPtr obj, ExprPtr func);
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBUNIT_H_ */
