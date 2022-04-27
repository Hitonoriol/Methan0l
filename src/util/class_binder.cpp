#include "class_binder.h"
#include <utility>

namespace mtl
{

void BinderTest::pair(ExprEvaluator &eval)
{
	auto binder = ClassBinder<std::pair<int, int>>("Pair", eval);

	/* Bind constructor by signature */
	binder.bind_constructor<int, int>();

	/* Bind a native method */
	binder.bind_method("swap", &CLASS(binder)::swap);

	/* Register a custom method */
	binder.register_method("show", [](Object &obj) {
		auto &p = OBJECT(binder, obj);
		out << "Pair: [" << p.first << ", " << p.second << "]" << NL;
	});

	binder.register_class();

	OUT("Bound a native class: " << type_name<CLASS(binder)>())
}

}
