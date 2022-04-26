#include "class_binder.h"
#include <utility>

namespace mtl
{

void BinderTest::pair(ExprEvaluator &eval)
{
	auto binder = ClassBinder<std::pair<int, int>>("Pair", eval);
	auto &pair = binder.get_class();
	pair.register_method("test", eval.bind_func([&](Object &ths) {
		auto &p = binder.as_native(ths);
		out << "Pair: [" << p.first << ", " << p.second << "]" << NL;
	}));
	binder.bind_constructor<int, int>();
	binder.register_class();
}

}
