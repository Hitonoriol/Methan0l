#ifndef SRC_UTIL_CONTAINERS_H_
#define SRC_UTIL_CONTAINERS_H_

#include <type_traits>

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

template<typename C>
inline auto pop(C &c)
{
	auto elem = c.top();
	c.pop();
	return elem;
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
	auto it = retain_in.begin(), end = retain_in.end();
	while(it != end) {
		if (!contains(lookup_in, *it))
			retain_in.erase(it++);
		else
			++it;
	}
	return retain_in;
}

}

#endif /* SRC_UTIL_CONTAINERS_H_ */
