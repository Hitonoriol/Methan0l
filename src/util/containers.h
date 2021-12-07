#ifndef SRC_UTIL_CONTAINERS_H_
#define SRC_UTIL_CONTAINERS_H_

#include <utility>

#include "meta.h"

namespace mtl
{

template<typename C, typename T>
inline auto insert(C &c, T &&val) -> decltype(c.push_back(std::forward<T>(val)), void())
{
	c.push_back(std::forward<T>(val));
}

template<typename C, typename T>
inline auto insert(C &c, T &&val) -> decltype(c.insert(std::forward<T>(val)), void())
{
	c.insert(std::forward<T>(val));
}

template<typename T, typename U>
inline U& add_all(T &from, U &to)
{
	static_assert(is_container<T>::value && is_container<U>::value
			&& std::is_same<typename T::value_type, typename U::value_type>::value);
	for (auto &elem : from)
		insert(to, elem);
	return to;
}

}

#endif /* SRC_UTIL_CONTAINERS_H_ */
