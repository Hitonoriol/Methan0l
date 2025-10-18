#ifndef LANG_UTIL_CONTAINERS_H_
#define LANG_UTIL_CONTAINERS_H_

#include "structure/Value.h"

#include <utility>

namespace mtl
{

template<typename Container, typename Tuple>
inline auto from_tuple(Tuple&& tuple)
{
	return std::apply([](auto &&... elems) {
		Container result;
		(result.push_back(std::forward<Value>(Value(elems))), ...);
		return result;
		}, std::forward<Tuple>(tuple));
}

}

#endif // LANG_UTIL_CONTAINERS_H_