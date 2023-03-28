#include "Iterable.h"

#include <oop/NativeClass.h>
#include <CoreLibrary.h>


namespace mtl
{

METHAN0L_CLASS_BODY(Iterable, {
	INTERFACE
	ABSTRACT_METHOD(iterator)

	/* Transformations */
	BIND_EXTERNAL_MUTATOR_METHOD(core::for_each)
	BIND_EXTERNAL_METHOD(core::map)
	BIND_EXTERNAL_METHOD(core::filter)
	BIND_EXTERNAL_METHOD(core::accumulate)
	BIND_EXTERNAL_METHOD(core::collect)

	/* Accumulative mathematical operations */
	BIND_EXTERNAL_METHOD(core::sum)
	BIND_EXTERNAL_METHOD(core::product)
	BIND_EXTERNAL_METHOD(core::mean)
	BIND_EXTERNAL_METHOD(core::rms)
	BIND_EXTERNAL_METHOD(core::deviation)
})

}
