#ifndef SRC_UTIL_CAST_H_
#define SRC_UTIL_CAST_H_

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <any>

#include "memory.h"
#include "string.h"
#include "meta/type_traits.h"
#include "meta/variant_traits.h"

namespace mtl
{

template<typename T>
inline T any_cast(std::any &any)
{
	using VType = TYPE(T);
	static_assert(std::is_reference_v<T> || std::is_copy_constructible_v<T>);
	static_assert(std::is_constructible_v<T, VType&>);

	if (auto ptr = std::any_cast<VType>(&any))
		return *ptr;

	throw std::runtime_error("Bad any access. "
			"Contained type: " + mtl::str(any.type().name())
			+ ", expected: " + mtl::str(type_name<T>()));
}

template<typename To, typename From>
inline To& try_cast(From *ptr)
{
	if (instanceof<To>(ptr))
		return static_cast<To&>(*ptr);

	throw std::runtime_error("Bad cast. "
			+ mtl::str(type_name<From*>())
			+ " is not convertible into: " + mtl::str(type_name<To*>()));
}

template<typename To, typename From>
inline To& try_cast(std::shared_ptr<From> ptr)
{
	return try_cast<To>(ptr.get());
}

/* (!) Returned value must not be mutated if declared const originally */
template<typename T, typename UC = typename std::remove_const<T>::type>
constexpr UC& unconst(T &val)
{
	return *const_cast<UC*>(&val);
}

template<typename T, typename ... Types, typename Variant = std::variant<Types...>>
constexpr T& get(std::variant<Types...> &variant)
{
	if (auto valptr = std::get_if<T>(&variant))
		return *valptr;

	auto contained = std::visit([](auto &v) -> std::string_view {
		using Contained = VT(v);
		IF (std::is_same_v<Contained, std::any>)
			return v.type().name();
		return type_name<Contained>();
	}, variant);
	throw std::runtime_error("Bad variant access. "
			"Contained type: " + mtl::str(contained)
			+ ", expected: " + mtl::str(type_name<T>()));
}

template<typename T, typename ... Types>
inline constexpr const T& get(const std::variant<Types...> &variant)
{
	return mtl::get<T>(unconst(variant));
}

}

#endif /* SRC_UTIL_CAST_H_ */
