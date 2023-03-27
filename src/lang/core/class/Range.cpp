#include "Range.h"

namespace mtl
{

NATIVE_CLASS_BINDING(IntRange, {
	RANGE_BINDINGS
})

NATIVE_CLASS_BINDING(IntRangeIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

NATIVE_CLASS_BINDING(FloatRange, {
	RANGE_BINDINGS
})

NATIVE_CLASS_BINDING(FloatRangeIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

namespace native
{

}

} /* namespace mtl */
