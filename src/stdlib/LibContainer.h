#ifndef SRC_LANG_CORE_LIBRARY_LIBCONTAINER_H_
#define SRC_LANG_CORE_LIBRARY_LIBCONTAINER_H_

#include <core/Library.h>

namespace mtl
{

class LibContainer: public Library
{
	public:
		using Library::Library;
		void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_LIBRARY_LIBCONTAINER_H_ */
