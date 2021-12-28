#ifndef SRC_UTIL_CAST_H_
#define SRC_UTIL_CAST_H_

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include "memory.h"

namespace mtl
{

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
inline To& try_cast(From *ptr)
{
	if (instanceof<To>(ptr))
		return static_cast<To&>(*ptr);

	std::stringstream err_ss;
	err_ss << "Invalid value type received: "
			<< type_name<From>()
			<< " (Expected: " << type_name<To>() << ")";
	throw std::runtime_error(err_ss.str());
}

template<typename To, typename From>
inline To& try_cast(std::shared_ptr<From> ptr)
{
	return try_cast<To>(ptr.get());
}

/* Super dangerous & dirty */
template<typename T, typename UC = typename std::remove_const<T>::type>
constexpr UC& unconst(T &val)
{
	return *const_cast<UC*>(&val);
}

}

#endif /* SRC_UTIL_CAST_H_ */
