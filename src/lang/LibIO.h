#ifndef SRC_LANG_LIBIO_H_
#define SRC_LANG_LIBIO_H_

#include "Library.h"

namespace mtl
{

class LibIO: public Library
{
	public:
		LibIO(ExprEvaluator *eval) : Library(eval) {}
		void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBIO_H_ */
