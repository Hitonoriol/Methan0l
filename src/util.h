#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <string>
#include <type_traits>
#include <functional>

template<typename Base, typename T>
inline bool instanceof(const T *ptr)
{
	return std::is_base_of<Base, T>::value || dynamic_cast<const Base*>(ptr) != nullptr;
}

inline std::string str(char chr)
{
	return std::string(1, chr);
}

inline std::string& lrstrip(std::string &str)
{
	str.erase(0, 1);
	str.erase(str.size() - 1, 1);
	return str;
}

template<typename To, typename From>
inline To& try_cast(std::shared_ptr<From> ptr)
{
	if (instanceof<To>(ptr.get()))
		return static_cast<To&>(*ptr);

	throw std::runtime_error("Invalid expression type received");
}

/* Create a "string list" representation of strings provided by <str_supplier> while its return is not empty */
inline std::string stringify(std::function<std::string(void)> str_supplier)
{
	std::string list_str = "{ ";

	std::string elem;
	while (!(elem = str_supplier()).empty())
		list_str += elem + ", ";
	list_str.erase(std::prev(list_str.end(), 2));

	list_str += "}";
	return list_str;
}

#endif /* UTIL_H_ */
