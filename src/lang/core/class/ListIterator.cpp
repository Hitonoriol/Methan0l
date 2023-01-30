#include "ListIterator.h"

namespace mtl
{

NATIVE_CLASS_INIT(ListIterator, {
	auto &binder = get_binder();
	BIND_CONSTRUCTOR(ValList&)
	BIND_METHOD(next)
	BIND_METHOD(has_next)
	BIND_METHOD(previous)
	BIND_METHOD(has_previous)
	BIND_METHOD(remove)
})

} /* namespace mtl */
