#include "Range.h"

namespace mtl
{

NATIVE_CLASS_BINDING(IntRange, {
	RANGE_BINDINGS
})

NATIVE_CLASS_BINDING(IntRangeIterator, {
	NON_CONSTRUCTIBLE
	ITERATOR_BINDINGS
})

NATIVE_CLASS_BINDING(FloatRange, {
	RANGE_BINDINGS
})

NATIVE_CLASS_BINDING(FloatRangeIterator, {
	NON_CONSTRUCTIBLE
	ITERATOR_BINDINGS
})

namespace native
{

}

} /* namespace mtl */
