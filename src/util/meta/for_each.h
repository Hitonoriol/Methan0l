#ifndef SRC_UTIL_META_FOR_EACH_H_
#define SRC_UTIL_META_FOR_EACH_H_

#include <utility>

namespace mtl
{

template<typename E, E... Elements>
struct StaticList
{
	template<class F>
	static constexpr void for_each(F &&f)
	{
		(f(std::integral_constant<E, Elements>()), ...);
	}
};

}
#endif /* SRC_UTIL_META_FOR_EACH_H_ */
