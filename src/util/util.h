#ifndef UTIL_H_
#define UTIL_H_

#include <deque>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <sstream>
#include <string_view>
#include <utility>

#include "type.h"
#include "expression/Expression.h"

#include "memory.h"
#include "array.h"
#include "cast.h"

namespace mtl
{

struct Value;

extern std::ostream &out;

template<typename T>
inline auto str(T val) -> typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
{
	return std::to_string(val);
}

inline std::string str(char chr)
{
	return std::string(1, chr);
}

inline std::string str(const std::string_view &sv)
{
	return std::string(sv);
}

inline std::string str(std::ostream &s)
{
	std::stringstream ss;
	ss << s.rdbuf();
	return ss.str();
}

inline void clear(std::stringstream &ss)
{
	std::stringstream().swap(ss);
}

inline bool ends_with(std::string_view str, std::string_view suffix)
{
	auto end = str.substr(str.size() - suffix.size());
	return end == suffix;
}

inline std::string& strip_quotes(std::string &str)
{
	if (str[0] != '"' && str[0] != '\'')
		return str;

	str.erase(0, 1);
	str.erase(str.size() - 1, 1);
	return str;
}

void replace_all(std::string &str, const std::string &from, const std::string &to,
		int limit = -1);
std::vector<std::string> split(const std::string &s, const std::string &delimiter);
std::string to_base(udec value, int base);

std::string read_file(const std::string &name);
std::string read_file(std::istream &file);

/* Create a "string list" representation of strings provided by <str_supplier> while its return is not empty */
inline std::string stringify(std::function<std::string(void)> str_supplier)
{
	std::string list_str = "{";

	std::string elem;
	while (!(elem = str_supplier()).empty())
		list_str += elem + ", ";

	if (list_str.size() > 1)
		list_str.erase(list_str.size() - 2);

	list_str += "}";
	return list_str;
}

template<typename T>
inline std::string stringify_container(ExprEvaluator *eval, const T &ctr)
{
	auto it = ctr.begin(), end = ctr.end();
	return stringify([&]() {
		if (it == end) return empty_string;
		return unconst(*(it++)).to_string(eval);
	});
}

template<typename T>
inline bool contains(const std::string &str, const T &needle)
{
	if (needle.size() > str.size())
		return false;
	return str.find(needle) != std::string::npos;
}

}

#endif /* UTIL_H_ */
