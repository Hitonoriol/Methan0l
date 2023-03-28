#include "Collection.h"

#include <lang/core/Data.h>

namespace mtl
{

NATIVE_CLASS_BINDING(Collection, {
	INTERFACE
	IMPLEMENTS(Iterable)
	COLLECTION_BINDINGS

	BIND_EXTERNAL_METHOD(core::add_all)
	BIND_EXTERNAL_METHOD(core::remove_all)
	BIND_EXTERNAL_METHOD(core::retain_all)
	BIND_EXTERNAL_DARGS_METHOD(core::fill, 0)
})

}
