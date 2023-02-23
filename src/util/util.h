#ifndef UTIL_H_
#define UTIL_H_

#include "type.h"
#include "expression/Expression.h"

#include "io.h"
#include "memory.h"
#include "array.h"
#include "cast.h"
#include "string.h"
#include "debug.h"
#include "filesystem.h"
#include "meta/type_traits.h"

namespace mtl
{

struct Value;

template<typename T>
inline std::string stringify_container(Interpreter *context, const T &ctr)
{
	auto it = ctr.begin(), end = ctr.end();
	return stringify([&]() {
		if (it == end) return empty_string;
		return unconst(*(it++)).to_string(context);
	});
}

}

#endif /* UTIL_H_ */
