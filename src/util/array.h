#ifndef SRC_UTIL_ARRAY_H_
#define SRC_UTIL_ARRAY_H_

#include <array>
#include <algorithm>
#include <functional>

template<typename A, typename T>
inline bool contains(const A &arr, const T &elem)
{
	auto end = std::end(arr);
	return std::find(std::begin(arr), end, elem) != end;
}

template<typename A, typename T>
inline T find(const A &arr, const T &elem, const T &default_elem)
{
	auto end = std::end(arr);
	auto pos = std::find(std::begin(arr), end, elem);
	if (pos == end)
		return default_elem;

	return *pos;
}

template<typename A, typename T>
inline void for_each(const A &arr, T &&action)
{
	std::for_each(std::begin(arr), std::end(arr), action);
}

template<typename T, typename F>
inline void for_each(const std::initializer_list<T> list, F &&action)
{
	std::for_each(std::begin(list), std::end(list), action);
}

template<typename T, typename F>
inline void for_each(F &&action, const std::initializer_list<T> list)
{
	std::for_each(std::begin(list), std::end(list), action);
}

#endif /* SRC_UTIL_ARRAY_H_ */
