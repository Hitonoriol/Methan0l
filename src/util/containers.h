#ifndef SRC_UTIL_CONTAINERS_H_
#define SRC_UTIL_CONTAINERS_H_

#include <utility>

#include "meta.h"

#define FWD_VAL std::forward<T>(val)
#define LIST_T decltype(c.push_back(FWD_VAL), void())
#define SET_T decltype(c.insert(FWD_VAL), void())

namespace mtl
{

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

template<typename T, typename U>
inline U& add_all(T &from, U &to)
{
	for (auto &&elem : from)
		insert(to, elem);
	return to;
}

template<typename T, typename U>
inline U& remove_all(T &lookup_in, U &remove_from)
{
	for (auto &&elem : lookup_in)
		remove(remove_from, elem);
	return remove_from;
}

template<typename T, typename U>
inline U& retain_all(T &lookup_in, U &retain_in)
{
	for (auto &&elem : lookup_in)
		if (!contains(retain_in, elem))
			remove(retain_in, elem);
	return retain_in;
}

}

#endif /* SRC_UTIL_CONTAINERS_H_ */
