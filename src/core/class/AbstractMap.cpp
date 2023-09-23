#include "AbstractMap.h"

namespace mtl
{

NATIVE_CLASS_BINDING(AbstractMap, {
	INTERFACE
	IMPLEMENTS(Iterable)
	ABSTRACT_MAP_BINDINGS
})

} /* namespace mtl */
