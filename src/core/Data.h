#ifndef SRC_LANG_CORE_DATA_H_
#define SRC_LANG_CORE_DATA_H_

#include <CoreLibrary.h>
#include <interpreter/Interpreter.h>
#include <expression/LiteralExpr.h>
#include <core/class/Collection.h>

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

Boolean contains(Object &iterable_obj, Value);
Int count(Object &iterable_obj);

Value for_each(Object &iterable_obj, Value action);
Value map(Object &iterable_obj, Value mapper);
Value filter(Object &iterable_obj, Value predicate);
Value accumulate(Object &iterable_obj, Value accumulator);
Value slice(Object &iterable_obj, Object range);

Value collect(Object &iterable_obj, Object collection_obj);

Float sum(Object &iterable_obj);
Float mean(Object &iterable_obj);
Float product(Object &iterable_obj);
Float rms(Object &iterable_obj);
Float deviation(Object &iterable_obj);

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

Value fill(Object &collection_obj, Value elem, size_t size);
Value add_all(Object &to_obj, Object from_obj);
Value remove_all(Object &remove_from_obj, Object lookup_in_obj);
Value retain_all(Object &retain_in_obj, Object lookup_in_obj);

}

#endif /* SRC_LANG_CORE_DATA_H_ */
