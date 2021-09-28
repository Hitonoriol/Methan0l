#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <string>
#include <type_traits>

template<typename Base, typename T>
inline bool instanceof(const T*)
{
	return std::is_base_of<T, Base>::value;
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

#endif /* UTIL_H_ */
