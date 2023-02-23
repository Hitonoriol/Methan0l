#ifndef SRC_LANG_CORE_DATA_H_
#define SRC_LANG_CORE_DATA_H_

#include "interpreter/Interpreter.h"
#include "expression/LiteralExpr.h"

namespace mtl::core
{

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
inline TYPE(T) slice(T &container, UInt start, UInt end, Int step = 1)
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

template<typename T>
void for_each(Interpreter &context, T &container, Function &action)
{
	static_assert(is_container<T>::value);
	if (container.empty())
		return;

	ExprList action_args;
	ValueRef valref;
	/* List-like types */
	if constexpr (!is_associative<T>::value) {
		auto elem_expr = LiteralExpr::empty();
		action_args.push_front(elem_expr);
		for (auto &elem : container) {
			valref.reset(unconst(elem));
			elem_expr->raw_ref() = valref;
			context.invoke(action, action_args);
		}
	}
	/* Map-like types */
	else {
		auto key = LiteralExpr::empty(), val = LiteralExpr::empty();
		action_args = { key, val };
		for (auto &entry : container) {
			key->raw_ref() = entry.first;
			valref.reset(entry.second);
			val->raw_ref() = valref;
			context.invoke(action, action_args);
		}
	}
}

}

#endif /* SRC_LANG_CORE_DATA_H_ */
