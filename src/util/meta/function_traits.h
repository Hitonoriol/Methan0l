#ifndef SRC_UTIL_META_FUNCTION_TRAITS_H_
#define SRC_UTIL_META_FUNCTION_TRAITS_H_

#include <type.h>

#include <type_traits>
#include <utility>
#include <functional>

namespace mtl
{

template<class F>
struct function_traits;

// function pointer
#define F_TRAIT_PTR(sig) 	template<class R, class ... Args>\
							struct function_traits<sig> : public function_traits<R(Args...)> {};
F_TRAIT_PTR(R(*)(Args...))
F_TRAIT_PTR(R(*const)(Args...))
F_TRAIT_PTR(R(*)(Args...,...))
F_TRAIT_PTR(R(*const)(Args...,...))

template<class R, class ... Args>
struct function_traits<R(Args...)>
{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template<std::size_t N>
		struct argument
		{
				static_assert(N < arity, "invalid parameter index");
				using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
};

// member function pointer
#define F_TRAIT_MEMBER(sig)		template<class C, class R, class ... Args>\
								struct function_traits<sig> : public function_traits<R(C&, Args...)>{};
F_TRAIT_MEMBER(R(C::*)(Args...))
F_TRAIT_MEMBER(R(C::*)(Args...)const)
F_TRAIT_MEMBER(R(C::*const&)(Args...))

#define MEMBER_WRAPPER(sig)		template<class C, typename R, typename ...Args>\
								constexpr auto member(C *instance, sig)\
								{\
									return [instance, func](Args ...args) -> R {return (instance->*func)(args...);};\
								}
MEMBER_WRAPPER(R(C::*func)(Args...))
MEMBER_WRAPPER(R(C::*func)(Args...)const)

// member object pointer
template<class C, class R>
struct function_traits<R (C::*)> : public function_traits<R(C&)>
{
};

// functor
template<class F>
struct function_traits
{
	private:
		using call_type = function_traits<decltype(&F::operator())>;
		public:
		using return_type = typename call_type::return_type;

		static constexpr std::size_t arity = call_type::arity - 1;

		template<std::size_t N>
		struct argument
		{
				static_assert(N < arity, "invalid parameter index");
				using type = typename call_type::template argument<N+1>::type;
		};
};

template<class F>
struct function_traits<F&> : public function_traits<F>
{
};

template<class F>
struct function_traits<F&&> : public function_traits<F>
{
};

}

#endif /* SRC_UTIL_META_FUNCTION_TRAITS_H_ */
