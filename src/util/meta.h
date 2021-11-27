#ifndef SRC_UTIL_META_H_
#define SRC_UTIL_META_H_

#include <type_traits>

/* is_equality_comparable<T, U>*/
template<typename T, typename U>
using equality_comparison_t = decltype(std::declval<T&>() == std::declval<U&>());

template<typename T, typename U, typename = std::void_t<>>
struct is_equality_comparable
: std::false_type
{
};

template<typename T, typename U>
struct is_equality_comparable<T, U, std::void_t<equality_comparison_t<T, U> > >
: std::is_same<equality_comparison_t<T, U>, bool>
{
};

#endif /* SRC_UTIL_META_H_ */
