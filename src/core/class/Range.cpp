#include "Range.h"

namespace mtl
{

METHAN0L_CLASS_BODY(Range, {
	INTERFACE
	IMPLEMENTS(Iterable)
	RANGE_BINDINGS
})

NATIVE_CLASS_BINDING(IntRange, {
	IMPLEMENTS_RANGE

	BIND_METHOD(to_string)
	BIND_METHOD(hash_code)
})

NATIVE_CLASS_BINDING(IntRangeIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

NATIVE_CLASS_BINDING(FloatRange, {
	IMPLEMENTS_RANGE

	BIND_METHOD(to_string)
	BIND_METHOD(hash_code)
})

NATIVE_CLASS_BINDING(FloatRangeIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

namespace native
{

}

} /* namespace mtl */
