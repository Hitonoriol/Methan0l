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

namespace mtl
{

struct Value;

template<typename T>
inline std::string stringify_container(ExprEvaluator *eval, const T &ctr)
{
	auto it = ctr.begin(), end = ctr.end();
	return stringify([&]() {
		if (it == end) return empty_string;
		return unconst(*(it++)).to_string(eval);
	});
}

}

#endif /* UTIL_H_ */
