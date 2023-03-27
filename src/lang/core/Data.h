#ifndef SRC_LANG_CORE_DATA_H_
#define SRC_LANG_CORE_DATA_H_

#include <interpreter/Interpreter.h>
#include <expression/LiteralExpr.h>
#include <CoreLibrary.h>
#include <lang/core/class/Collection.h>

namespace mtl
{

using FloatOperator = const std::function<Float(Float, Float)>;

}

namespace mtl::core
{

using Initializer = std::function<void(ExprList&)>;

template<typename F>
inline Value do_for_each(IterableAdapter iterable, F action, Initializer init = [](ExprList&){})
{
	auto &context = iterable.get_context();
	auto it = iterable.iterator();
	auto elem_expr = LiteralExpr::empty();
	ExprList action_args { elem_expr };
	init(action_args);
	while (it.has_next()) {
		elem_expr->raw_ref() = it.next();
		action(context, action_args);
	}
	return iterable.get_object();
}

Value for_each(Object &obj, Value action);
Value map(Object &obj, Value mapper);
Value accumulate(Object &obj, Value accumulator);
Value slice(Object &obj, Object &range);

Value collect(Object &iterable_obj, Object collection_obj);

Float sum(Object &iterable);
Float mean(Object &iterable);
Float product(Object &iterable);
Float rms(Object &iterable);
Float deviation(Object &iterable);

template<typename T, typename F>
std::pair<T, UInt> do_accumulate(IterableAdapter iterable, T init, F &&operation)
{
	UInt size = 0;
	auto it = iterable.iterator();
	while(it.has_next()) {
		init = operation(init, it.next().as<T>());
		++size;
	}
	return {init, size};
}

Value range(Interpreter &context, Value start, Value end, Value step);

}

#endif /* SRC_LANG_CORE_DATA_H_ */
