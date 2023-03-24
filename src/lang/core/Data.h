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

Value slice(CollectionAdapter collection, Int start, Int end, Int step);

ValList range(Int start, Int n, Int step = 1, bool inclusive = false);

inline void check_range(Int start, Int end, Int step) {
	if ((end <= start && step > 0) || (end >= start && step < 0)
			/* Boundless ranges */
			|| (end > start && step < 0) || (end < start && step > 0) || step == 0)
		throw std::runtime_error("Invalid range");
}

inline bool rng_pos_cond(Int i, Int end)
{
	return i < end;
}

inline bool rng_neg_cond(Int i, Int end)
{
	return i > end;
}

template<typename T>
inline TYPE(T) do_slice(T &container, UInt start, UInt end, Int step = 1)
{
	check_range(start, end, step);
	end += step > 0 ? 1 : -1;
	TYPE(T) sliced;
	if (end > container.size() || start >= container.size())
		throw std::runtime_error("Specified sublist is out of bounds");
	auto condition = step > 0 ? rng_pos_cond : rng_neg_cond;
	for (auto i = start; condition(i, end); i += step)
		sliced.push_back(container[i]);
	return sliced;
}

}

#endif /* SRC_LANG_CORE_DATA_H_ */
