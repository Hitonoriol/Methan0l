#include <lang/core/Data.h>

namespace mtl::core
{

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
