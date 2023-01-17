#include <lang/core/Data.h>

namespace mtl::core
{

auto check_range = [](dec start, dec end, dec step) {
	if ((end <= start && step > 0) || (end >= start && step < 0)
			/* Boundless ranges */
			|| (end > start && step < 0) || (end < start && step > 0) || step == 0)
		throw std::runtime_error("Invalid range");
};

auto rng_pos_cond = [](dec i, dec end) {return i < end;};
auto rng_neg_cond = [](dec i, dec end) {return i > end;};

Value range(dec start, dec end, dec step, bool inclusive)
{
	check_range(start, end, step);

	if (inclusive)
		end += step > 0 ? 1 : -1;

	auto condition = step > 0 ? rng_pos_cond : rng_neg_cond;
	Value listv(Type::LIST);
	ValList &list = listv.get<ValList>();
	for (dec i = start; condition(i, end); i += step)
		list.push_back(i);
	return listv;
}

Value slice(Value &containerv, udec start, udec end, dec step)
{
	check_range(start, end, step);
	end += step > 0 ? 1 : -1;
	Value sliced;
	containerv.accept([&](auto &ctr) {
		IF (Value::is_list_type<VT(ctr)>()) {
			if (end > ctr->size() || start >= ctr->size())
				throw std::runtime_error("Specified sublist is out of bounds");

			VT(*ctr) slice;
			auto condition = step > 0 ? rng_pos_cond : rng_neg_cond;
			for (udec i = start; condition(i, end); i += step)
				slice.push_back((*ctr)[i]);
			sliced = slice;
		}
	});
	return sliced;
}

}
