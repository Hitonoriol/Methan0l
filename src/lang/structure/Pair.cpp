#include "Pair.h"

#include <type.h>
#include <util/cast.h>
#include <util/class_binder.h>
#include <util/debug.h>
#include <util/io.h>
#include <structure/object/Object.h>

#include <iostream>
#include <utility>

namespace mtl
{

void Pair::initialize()
{
	auto &binder = get_binder();
	binder.set_name("Pair");
	binder.bind_constructor<Value, Value>();

	binder.bind_method("swap", &CLASS(binder)::swap);

	binder.register_method("swap_contents", [](Object &obj) {
		auto &p = OBJECT(binder, obj);
		std::swap(p.first, p.second);
	});

	binder.register_method("x", [](Object &obj) {
		return Value::ref(OBJECT(binder, obj).first);
	});

	binder.register_method("y", [](Object &obj) {
		return Value::ref(OBJECT(binder, obj).second);
	});

	binder.register_method("show", [](Object &obj) {
		auto &p = OBJECT(binder, obj);
		out << "Pair: [" << p.first << ", " << p.second << "]" << NL;
	});

	binder.register_class();

	LOG("Bound a native class: " << type_name<bound_class>() << " [" << binder.get_class().get_name() << "]")
}

} /* namespace mtl */
