#include "Pair.h"

#include <iostream>
#include <utility>

#include <type.h>
#include <util/cast.h>
#include <util/class_binder.h>
#include <util/debug.h>
#include <util/io.h>
#include <structure/object/Object.h>

namespace mtl
{

NATIVE_CLASS_BINDING(Pair, {
	BIND_CONSTRUCTOR(Value, Value)
	BIND_METHOD(swap)

	auto &binder = get_binder();
	binder.register_method("swap_contents", METHOD(OBJ) {
		auto &p = THIS;
		std::swap(p.first, p.second);
	});

	binder.register_method("x", METHOD(OBJ) {
		return Value::ref(THIS.first);
	});

	binder.register_method("y", METHOD(OBJ) {
		return Value::ref(THIS.second);
	});

	binder.register_method(Methods::TO_STRING, METHOD(OBJ) {
		auto &p = THIS;
		return std::string("{Pair: [" + p.first.to_string() + ", " + p.second.to_string() + "]}");
	});
})

} /* namespace mtl */
