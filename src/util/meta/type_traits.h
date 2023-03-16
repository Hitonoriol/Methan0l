#ifndef SRC_UTIL_META_TYPE_TRAITS_H_
#define SRC_UTIL_META_TYPE_TRAITS_H_

#include <type_traits>
#include <utility>
#include <iterator>

namespace mtl
{

template<typename ...Ts>
constexpr auto type_name()
{
	std::string_view name, prefix, suffix;
#ifdef __clang__
	name = __PRETTY_FUNCTION__;
	prefix = "auto type_name() [Ts = ";
	suffix = "]";
#elif defined(__GNUC__)
	name = __PRETTY_FUNCTION__;
	prefix = "constexpr auto type_name() [with Ts = ";
	suffix = "]";
#elif defined(_MSC_VER)
	name = __FUNCSIG__;
	prefix = "auto __cdecl type_name<";
	suffix = ">(void)";
#endif
	name.remove_prefix(prefix.size());
	name.remove_suffix(suffix.size());
	return name;
}

/* is_equality_comparable<T, U>*/
template<typename T, typename U>
using equality_comparison_t = decltype(std::declval<T&>() == std::declval<U&>());

template<typename T, typename U, typename = std::void_t<>>
struct is_equality_comparable
: std::false_type
{
};

template<typename T, typename U>
struct is_equality_comparable<T, U, std::void_t<equality_comparison_t<T, U>>>
: std::is_same<equality_comparison_t<T, U>, bool>
{
};

template<typename T>
struct has_const_iterator
{
private:
	typedef char					  yes;
	typedef struct { char array[2]; } no;

	template<typename C> static yes test(typename C::const_iterator*);
	template<typename C> static no  test(...);

public:
	static const bool value = sizeof(test<T>(0)) == sizeof(yes);
	typedef T type;
};

template <typename T>
struct has_begin_end
{
	template<typename C>
	static char (&f(typename std::enable_if<
			std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
			typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

	template<typename C> static char (&f(...))[2];

	template<typename C> static char (&g(typename std::enable_if<
			std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
			typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

	template<typename C> static char (&g(...))[2];

	static bool const beg_value = sizeof(f<T>(0)) == 1;
	static bool const end_value = sizeof(g<T>(0)) == 1;
};

template<typename T>
struct is_container: std::integral_constant<bool,
has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value && !std::is_same<T, std::string>::value
>
{
};

/* is_associative<T> */
template<typename T, typename U = void>
struct is_associative: std::false_type
{
};

template<typename T>
struct is_associative<T, std::void_t<typename T::key_type,
		typename T::mapped_type,
		decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])>>
: std::true_type
{
};

template<typename T>
struct is_callable
{
	private:
		typedef char (&yes)[1];
		typedef char (&no)[2];

		struct Fallback
		{
				void operator()();
		};

		struct Derived: T, Fallback {};

		template<typename U, U> struct Check;

		template<typename >
		static yes test(...);

		template<typename C>
		static no test(Check<void (Fallback::*)(), &C::operator()>*);

	public:
		static const bool value = sizeof(test<Derived>(0)) == sizeof(yes);
};

/* Defines a SFINAE member type detector.
 * `name` - detector struct name
 * `member_type_name` - member type name to test for */
#define MEMBER_TYPE_DETECTOR(name, member_type_name) \
		template<class, class = void> struct name : std::false_type {}; \
		template<class T> struct name<T, std::void_t<typename T::member_type_name>> : std::true_type {};

/* Checks whether supplied type T is a class binding
 * (if it defines a `bound_class` public member type) */
MEMBER_TYPE_DETECTOR(is_class_binding, bound_class)
MEMBER_TYPE_DETECTOR(is_class_wrapper, wrapped_type)

template<typename Iter>
struct is_reverse_iterator: std::false_type {};

template<typename Iter>
struct is_reverse_iterator<std::reverse_iterator<Iter>>
: std::integral_constant<bool, !is_reverse_iterator<Iter>::value> {};

}

#endif /* SRC_UTIL_META_TYPE_TRAITS_H_ */
