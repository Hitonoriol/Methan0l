#ifndef SRC_LANG_LIBLOGICAL_H_
#define SRC_LANG_LIBLOGICAL_H_

#include <core/Library.h>

namespace mtl
{

class LibLogical: public Library
{
	public:
		using Library::Library;
		void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBLOGICAL_H_ */
