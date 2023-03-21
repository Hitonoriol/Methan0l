#include "Collection.h"

namespace mtl
{

NATIVE_CLASS_BINDING(Collection, {
	INTERFACE
	IMPLEMENTS(Iterable)
	COLLECTION_BINDINGS
})

}
