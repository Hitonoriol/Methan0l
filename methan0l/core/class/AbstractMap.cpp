#include "AbstractMap.h"

#include "Collection.h"
#include "DefaultIterator.h"
#include "GenericContainer.h"

#include <methan0l/oop/NativeClass.h>
#include <methan0l/structure/Wrapper.h>
#include <methan0l/core/Data.h>

namespace mtl
{

NATIVE_CLASS_BINDING(AbstractMap, {
	INTERFACE
	IMPLEMENTS(Iterable)
	ABSTRACT_MAP_BINDINGS
})

} /* namespace mtl */
