#include "DefaultIterator.h"

namespace mtl
{

NATIVE_CLASS_BINDING(ListIterator, {
	ITERATOR_BINDINGS(ValList)
})

NATIVE_CLASS_BINDING(SetIterator, {
	ITERATOR_BINDINGS(ValSet)
})

}
