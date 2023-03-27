#include "DefaultIterator.h"

namespace mtl
{

NATIVE_CLASS_BINDING(ListIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

NATIVE_CLASS_BINDING(SetIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

}
