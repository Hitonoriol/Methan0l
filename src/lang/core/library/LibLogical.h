#ifndef SRC_LANG_LIBLOGICAL_H_
#define SRC_LANG_LIBLOGICAL_H_

#include "lang/Library.h"

namespace mtl
{

class LibLogical: public Library
{
	public:
		LibLogical(Interpreter *context) : Library(context) {}
		void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBLOGICAL_H_ */
