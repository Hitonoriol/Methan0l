#include "Pair.h"

#include <iostream>
#include <utility>

#include <type.h>
#include <util/cast.h>
#include <util/class_binder.h>
#include <util/debug.h>
#include <util/io.h>
#include <oop/Object.h>
#include <CoreLibrary.h>

namespace mtl
{

NATIVE_CLASS_BINDING(Pair, {
	BIND_CONSTRUCTOR(Value, Value)
	BIND_METHOD(swap)

	METHOD(swap_contents) (OBJ) {
		auto &p = THIS;
		std::swap(p.first, p.second);
	};

	METHOD(x) (OBJ) {
		return Value::ref(THIS.first);
	};

	METHOD(y) (OBJ) {
		return Value::ref(THIS.second);
	};

	STANDARD_METHOD(Methods::ToString) (OBJ) {
		auto &p = THIS;
		return CONTEXT.make<String>(
			"{Pair: [" + mtl::str(p.first.to_string()) + ", "
			+ mtl::str(p.second.to_string()) + "]}"
		);
	};
})

} /* namespace mtl */
