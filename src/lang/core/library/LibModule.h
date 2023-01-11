#ifndef SRC_LANG_CORE_LIBMODULE_H_
#define SRC_LANG_CORE_LIBMODULE_H_

#include <lang/Library.h>
#include <boost/dll.hpp>

namespace mtl
{

class Unit;

class LibModule: public Library
{
	public:
		using Library::Library;
		virtual void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_LIBMODULE_H_ */
