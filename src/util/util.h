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

#include "../type.h"
#include "../expression/Expression.h"

#include "memory.h"
#include "array.h"

namespace mtl
{

struct Value;

extern std::ostream &out, &log;

template<typename T>
constexpr auto type_name()
{
	std::string_view name, prefix, suffix;
#ifdef __clang__
	name = __PRETTY_FUNCTION__;
	prefix = "auto type_name() [T = ";
	suffix = "]";
#elif defined(__GNUC__)
	name = __PRETTY_FUNCTION__;
	prefix = "constexpr auto type_name() [with T = ";
	suffix = "]";
#elif defined(_MSC_VER)
	name = __FUNCSIG__;
	prefix = "auto __cdecl type_name<";
	suffix = ">(void)";
#endif
	name.remove_prefix(prefix.size());
	name.remove_suffix(suffix.size());
	auto begin = name.find("mtl::");
	return begin == std::string_view::npos ? name : name.substr(begin);
}

template<typename To, typename From>
inline To& try_cast(std::shared_ptr<From> ptr)
{
	if (instanceof<To>(ptr.get()))
		return static_cast<To&>(*ptr);

	std::stringstream err_ss;
	err_ss << "Invalid expression type received";
	if (instanceof<Expression>(ptr)) {
		static_cast<Expression&>(*ptr).info(err_ss << ": ");
		err_ss << " (Expected: " << type_name<To>() << ")";
	}

	throw std::runtime_error(err_ss.str());
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
std::deque<Value> split(const std::string &s, const std::string &delimiter);
std::string to_base(unsigned int value, int base);

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

}

#endif /* UTIL_H_ */
