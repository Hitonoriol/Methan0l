#include "AbstractMap.h"

#include "Collection.h"
#include "DefaultIterator.h"
#include "GenericContainer.h"

#include <oop/NativeClass.h>
#include <structure/Wrapper.h>
#include <core/Data.h>

namespace mtl
{

NATIVE_CLASS_BINDING(AbstractMap, {
	INTERFACE
	IMPLEMENTS(Iterable)
	ABSTRACT_MAP_BINDINGS
})

} /* namespace mtl */
