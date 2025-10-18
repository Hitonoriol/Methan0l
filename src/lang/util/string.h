#ifndef LANG_UTIL_STRING_H_
#define LANG_UTIL_STRING_H_

#include "type.h"

#include <string>

namespace mtl
{

template<typename T>
inline std::string stringify_container(const T& ctr)
{
	auto it = std::begin(ctr), end = std::end(ctr);
	return stringify([&] {
		if (it == end) {
			return std::string();
		}

		return mtl::str(unconst(*(it++)).to_string());
	});
}

}
#endif // LANG_UTIL_STRING_H_