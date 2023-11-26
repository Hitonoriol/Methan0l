#include "Data.h"

#include <cmath>

#include <CoreLibrary.h>
#include <core/class/String.h>

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

Boolean contains(Object &iterable_obj, Value value)
{
	IterableAdapter iterable(iterable_obj);
	auto it = iterable.iterator();
	while (it.has_next()) {
		if (it.next() == value)
			return true;
	}
	return false;
}

Int count(Object &iterable_obj)
{
	IterableAdapter iterable(iterable_obj);
	auto it = iterable.iterator();
	Int count = 0;
	while(it.has_next()) {
		it.next();
		++count;
	}
	return count;
}

Value for_each(Object &iterable_obj, Value action)
{
	return do_for_each(iterable_obj, [&action](auto &context, auto &args) {
		context.invoke(action, args);
	});
}

Value map(Object &iterable_obj, Value mapper)
{
	auto &context = iterable_obj.context();
	return context.make<Mapping>(iterable_obj, mapper);
}

Value filter(Object &iterable_obj, Value predicate)
{
	auto &context = iterable_obj.context();
	return context.make<Filter>(iterable_obj, predicate);
}

Value accumulate(Object &iterable_obj, Value accumulator)
{
	auto &context = iterable_obj.context();
	Value result = 0.0;
	do_for_each(
		iterable_obj,
		[&result, &accumulator](auto &context, auto &args) {
			result = context.invoke(accumulator, args);
		},
		/* Initializer: modify arg list so that the `accumulator`
		 * is called for a pair of arguments. */
		[&context, &result](auto &args) {
			args.push_front(Value::wrapped(&context, Value::ref(result)));
		}
	);
	return result;
}

Float sum(Object &iterable_obj)
{
	return do_accumulate(iterable_obj, 0.0, &summator).first;
}

Float product(Object &iterable_obj)
{
	return do_accumulate(iterable_obj, 1.0, &multiplicator).first;
}

Float mean(Object &iterable_obj)
{
	auto [sum, n] = do_accumulate(iterable_obj, 0.0, &summator);
	return sum / n;
}

Float rms(Object &iterable_obj)
{
	auto [dsum, n] = do_accumulate(iterable_obj, 0.0, [](Float l, Float r) {
		return l + r * r;
	});
	return sqrt(dsum / n);
}

Float deviation(Object &iterable_obj)
{
	double mean = core::mean(iterable_obj);
	auto [dsum, n] = do_accumulate(iterable_obj, 0.0, [mean](Float l, Float r) {
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

Value slice(Object &iterable_obj, Object range_obj)
{
	CollectionAdapter collection(iterable_obj);
	CollectionAdapter sliced(iterable_obj.get_class()->create());

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

Value collect(Object &iterable_obj, Object collection_obj)
{
	IterableAdapter iterable(iterable_obj);
	CollectionAdapter collection(collection_obj);
	auto it = iterable.iterator();
	while(it.has_next())
		collection.add(it.next());
	return collection_obj;
}

Value range(Interpreter &context, Value start, Value end, Value step)
{
	if (start.is<Float>() || end.is<Float>() || step.is<Float>())
		return context.make<FloatRange>(start.as<Float>(), end.as<Float>(), step.as<Float>());
	else
		return context.make<IntRange>(start.as<Int>(), end.as<Int>(), step.as<Int>());
}

Value retain_all(Object &retain_in_obj, Object lookup_in_obj)
{
	IterableAdapter lookup_in(lookup_in_obj);
	CollectionAdapter retain_in(retain_in_obj);

	Value elem;
	auto it = retain_in.iterator();
	while(it.has_next()) {
		elem = it.next();
		if (lookup_in.contains(elem))
			retain_in.remove(elem);
	}
	return retain_in;
}

Value remove_all(Object &remove_from_obj, Object lookup_in_obj)
{
	IterableAdapter lookup_in(lookup_in_obj);
	CollectionAdapter remove_from(remove_from_obj);

	auto it = lookup_in.iterator();
	while (it.has_next())
		remove_from.remove(it.next());

	return remove_from;
}

Value add_all(Object &to_obj, Object from_obj)
{
	IterableAdapter from(from_obj);
	CollectionAdapter to(to_obj);

	auto it = from.iterator();
	while (it.has_next())
		to.add(it.next());

	return to;
}

Value fill(Object &collection_obj, Value elem, size_t size)
{
	CollectionAdapter collection(collection_obj);

	if (size != 0)
		collection.resize(size);

	auto it = collection.iterator();
	while(it.has_next())
		it.next() = elem;

	return collection;
}

}
