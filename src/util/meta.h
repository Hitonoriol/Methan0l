#ifndef SRC_UTIL_META_H_
#define SRC_UTIL_META_H_

#include <type_traits>
#include <utility>
#include <iterator>

namespace mtl
{

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
struct get_arity: get_arity<decltype(&T::operator())>
{
};
template<typename R, typename ... Args>
struct get_arity<R (*)(Args...)> : std::integral_constant<unsigned, sizeof...(Args)>
{
};
template<typename R, typename C, typename ... Args>
struct get_arity<R (C::*)(Args...)> :
		std::integral_constant<unsigned, sizeof...(Args)>
{
};
template<typename R, typename C, typename ... Args>
struct get_arity<R (C::*)(Args...) const > :
		std::integral_constant<unsigned, sizeof...(Args)>
{
};

template<int...> struct seq
{
	using type = seq;
};
template<typename T1, typename T2> struct concat;
template<int ... I1, int ... I2> struct concat<seq<I1...>, seq<I2...>> : seq<I1...,
		(sizeof...(I1) + I2)...>
{
};

template<int N> struct gen_seq;
template<int N> struct gen_seq: concat<typename gen_seq<N / 2>::type,
		typename gen_seq<N - N / 2>::type>::type
{
};
template<> struct gen_seq<0> : seq<>
{
};
template<> struct gen_seq<1> : seq<0>
{
};

}

#endif /* SRC_UTIL_META_H_ */
