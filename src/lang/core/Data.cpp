#include "Data.h"

#include <cmath>

namespace mtl::core
{

Float summator(Float l, Float r)
{
	return l + r;
}

Float multiplicator(Float l, Float r)
{
	return l * r;
}

Value for_each(Object &obj, Value action)
{
	return do_for_each(obj, [&action](auto &context, auto &args) {
		context.invoke(action, args);
	});
}

Value map(Object &obj, Value mapper)
{
	auto mapped = CollectionAdapter(obj.context().make<List>());
	do_for_each(obj, [&mapper, &mapped](auto &context, auto &args) {
		mapped.add(context.invoke(mapper, args));
	});
	return mapped.get_object();
}

Value accumulate(Object &obj, Value accumulator)
{
	Value result = 0.0;
	do_for_each(
		obj,
		[&result, &accumulator](auto &context, auto &args) {
			result = context.invoke(accumulator, args);
		},
		/* Initializer: modify arg list so that the `accumulator`
		 * is called for a pair of arguments. */
		[&result](auto &args) {
			args.push_front(Value::wrapped(Value::ref(result)));
		}
	);
	return result;
}

Float sum(Object &iterable)
{
	return do_accumulate(iterable, 0.0, &summator).first;
}

Float product(Object &iterable)
{
	return do_accumulate(iterable, 1.0, &multiplicator).first;
}

Float mean(Object &iterable)
{
	auto [sum, n] = do_accumulate(iterable, 0.0, &summator);
	return sum / n;
}

Float rms(Object &iterable)
{
	auto [dsum, n] = do_accumulate(iterable, 0.0, [](Float l, Float r) {
		return l + r * r;
	});
	return sqrt(dsum / n);
}

Float deviation(Object &iterable)
{
	double mean = core::mean(iterable);
	auto [dsum, n] = do_accumulate(iterable, 0.0, [mean](Float l, Float r) {
		return l + (r - mean) * (r - mean);
	});
	return sqrt(dsum / n);
}

bool rng_pos_cond(Int i, Int end)
{
	return i < end;
}

bool rng_neg_cond(Int i, Int end)
{
	return i > end;
}

Value slice(Object &obj, Object &range_obj)
{
	CollectionAdapter collection(obj);
	CollectionAdapter sliced(obj.get_class()->create());

	auto &range = range_obj.get_native().get<IntRange>();
	Int start = range.get_start();
	Int end = range.get_end();
	Int step = range.get_step();

	if (static_cast<UInt>(end) > collection.size() || static_cast<UInt>(start) >= collection.size())
		throw std::runtime_error("Slice is out of bounds");

	auto condition = step > 0 ? rng_pos_cond : rng_neg_cond;
	for (auto i = start; condition(i, end); i += step)
		sliced.add(collection.get(i));

	return sliced;
}

Value range(Interpreter &context, Value start, Value end, Value step)
{
	if (start.is<Float>() || end.is<Float>() || step.is<Float>())
		return context.make<FloatRange>(start.as<Float>(), end.as<Float>(), step.as<Float>());
	else
		return context.make<IntRange>(start.as<Int>(), end.as<Int>(), step.as<Int>());
}

}
