#ifndef SRC_UTIL_STRING_H_
#define SRC_UTIL_STRING_H_

#include <type.h>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <cwchar>

namespace mtl
{

constexpr std::string_view TAB_S = "  |  ";

/* Keeps indentation levels after new line when using `\t` character */
std::string tab(std::string&&);
std::string tab(std::ostream&);
std::string indent(std::string&&);

void replace_all(std::string &str, std::string_view from, const std::string &to,
		int limit = -1);
std::vector<std::string> split(const std::string &s, const std::string &delimiter);
std::string to_base(udec value, uint8_t base);

template<typename T>
inline auto str(
		T val) -> typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
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

inline size_t str_length(const std::string &str)
{
	return str.length() - std::count_if(str.begin(), str.end(),
			[](char c) -> bool {return (c & 0xC0) == 0x80;});
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

template<typename T>
inline bool contains(const std::string &str, const T &needle)
{
	if (needle.size() > str.size())
		return false;
	return str.find(needle) != std::string::npos;
}

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

#endif /* SRC_UTIL_STRING_H_ */
