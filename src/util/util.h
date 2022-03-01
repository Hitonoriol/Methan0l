#ifndef UTIL_H_
#define UTIL_H_

#include <sstream>
#include <string>

#include "type.h"
#include "expression/Expression.h"

#include "memory.h"
#include "array.h"
#include "cast.h"
#include "string.h"

namespace mtl
{

struct Value;

extern std::ostream &out;

std::string read_file(const std::string &name);
std::string read_file(std::istream &file);

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
