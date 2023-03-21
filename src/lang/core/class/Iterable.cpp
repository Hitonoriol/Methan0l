#include "Iterable.h"

#include <oop/NativeClass.h>
#include <CoreLibrary.h>


namespace mtl
{

METHAN0L_CLASS_BODY(Iterable, {
	INTERFACE
	ABSTRACT_METHOD(iterator)

	BIND_EXTERNAL_METHOD(core::for_each)
	BIND_EXTERNAL_METHOD(core::map)
	BIND_EXTERNAL_METHOD(core::accumulate)
})

}
