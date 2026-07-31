#include "Iterable.h"

#include <methan0l/oop/NativeClass.h>
#include <methan0l/CoreLibrary.h>

namespace mtl
{

METHAN0L_CLASS_BODY(Iterable, {
	INTERFACE
	ABSTRACT_METHOD(iterator)

	/* Transformations */
	class_binder.bind_external_mutator_method(STR(core::for_each), static_cast<Value(*)(Object&, Value)>(&core::for_each));
	METHOD_ALIAS(for_each, IndexOperator::Foreach)
	BIND_EXTERNAL_METHOD(core::map)
	BIND_EXTERNAL_METHOD(core::filter)
	BIND_EXTERNAL_METHOD(core::accumulate)
	BIND_EXTERNAL_METHOD(core::collect)

	BIND_EXTERNAL_METHOD(core::contains)
	BIND_EXTERNAL_METHOD(core::count)

	/* Accumulative mathematical operations */
	BIND_EXTERNAL_METHOD(core::sum)
	BIND_EXTERNAL_METHOD(core::product)
	BIND_EXTERNAL_METHOD(core::mean)
	BIND_EXTERNAL_METHOD(core::rms)
	BIND_EXTERNAL_METHOD(core::deviation)
})

}
