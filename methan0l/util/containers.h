#ifndef SRC_UTIL_CONTAINERS_H_
#define SRC_UTIL_CONTAINERS_H_

#include <type_traits>
#include <utility>
#include <cstdint>

#define FWD_VAL std::forward<T>(val)
#define LIST_T decltype(c.push_back(FWD_VAL), void())
#define SET_T decltype(c.insert(FWD_VAL), void())

namespace mtl
{

template<typename C>
using iterator_of = decltype(std::begin(std::declval<C&>()));

template<typename C, typename T>
inline auto insert(C &c, T &&val) -> LIST_T
{
	c.push_back(FWD_VAL);
}

template<typename C, typename T>
inline auto remove(C &c, T &&val) -> LIST_T
{
	c.erase(std::remove(c.begin(), c.end(), FWD_VAL), c.end());
}

template<typename C>
inline auto remove(C &c, iterator_of<C> &it)
{
	c.erase(it);
}

template<typename C, typename T>
inline auto insert(C &c, T &&val) -> SET_T
{
	c.insert(FWD_VAL);
}

template<typename C, typename T>
inline auto remove(C &c, T &&val) -> SET_T
{
	c.erase(FWD_VAL);
}

template<typename C, typename T>
inline bool contains(C &c, T &&val)
{
	return std::find(std::begin(c), std::end(c), val) != std::end(c);
}

template<typename C>
inline auto pop(C &c)
{
	auto elem = c.top();
	c.pop();
	return elem;
}

template<typename T, typename V>
inline int64_t index_of(T&& container, V&& value)
{
	int64_t idx = 0;
	for (auto &&elem : container) {
		if (value == elem)
			return idx;
	}
	return -1;
}

template<typename C>
int64_t value_container_hash_code(C &&container)
{
	int64_t hash = container.size();
	for (auto &v : container)
		hash ^= v.hash_code() + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

}

#endif /* SRC_UTIL_CONTAINERS_H_ */
