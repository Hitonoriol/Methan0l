#ifndef SRC_LANG_LIBDATA_H_
#define SRC_LANG_LIBDATA_H_

#include "Library.h"

namespace mtl
{

/* Data structure operators & functions */
class LibData: public Library
{
	public:
		LibData(ExprEvaluator *eval) : Library(eval) {}
		void load() override;

	private:
		void load_operators();
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBDATA_H_ */
