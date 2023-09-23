#include "Iterator.h"

namespace mtl
{

METHAN0L_CLASS_BODY(Iterator, {
	INTERFACE
	ABSTRACT_METHOD(peek)
	ABSTRACT_METHOD(next)
	ABSTRACT_METHOD(has_next)
	ABSTRACT_METHOD(skip)
	ABSTRACT_METHOD(can_skip)
	ABSTRACT_METHOD(reverse)
	ABSTRACT_METHOD(previous)
	ABSTRACT_METHOD(has_previous)
	ABSTRACT_METHOD(remove)
})

}
