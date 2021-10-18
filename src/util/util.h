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

#include "../type.h"
#include "../expression/Expression.h"

namespace mtl
{

struct Value;

extern const std::ostream &out;

template<typename Base, typename T>
inline bool instanceof(const T *ptr)
{
	return std::is_base_of<Base, T>::value || dynamic_cast<const Base*>(ptr) != nullptr;
}

template<typename To, typename From>
inline To& try_cast(std::shared_ptr<From> ptr)
{
	if (instanceof<To>(ptr.get()))
		return static_cast<To&>(*ptr);

	std::stringstream err_ss;
	err_ss << "Invalid expression type received: ";
	if (instanceof<Expression>(ptr.get()))
		static_cast<Expression&>(*ptr).info(err_ss);

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

inline std::string& lrstrip(std::string &str)
{
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
