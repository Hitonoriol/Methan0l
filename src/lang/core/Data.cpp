#include <lang/core/Data.h>

namespace mtl::core
{

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

Value slice(CollectionAdapter collection, Int start, Int end, Int step)
{
	auto sliced = collection.get_object().get_class()->create();
	auto it = collection.iterator();
	/* TODO */
	return sliced;
}

ValList range(Int start, Int end, Int step, bool inclusive)
{
	check_range(start, end, step);

	if (inclusive)
		end += step > 0 ? 1 : -1;

	auto condition = step > 0 ? rng_pos_cond : rng_neg_cond;
	ValList list;
	for (Int i = start; condition(i, end); i += step)
		list.push_back(i);
	return list;
}

}
