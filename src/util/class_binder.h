#ifndef SRC_UTIL_CLASS_BINDER_H_
#define SRC_UTIL_CLASS_BINDER_H_

#include "ExprEvaluator.h"
#include "structure/object/Class.h"

#include <memory>

namespace mtl
{

template<class T, typename ... Types>
struct FactoryImpl
{
	using tuple_type = std::tuple<Types...>;
	template<typename>
	struct OptCallHelper;
	template<typename ... Args>
	struct OptCallHelper<std::tuple<Args...>>
	{
		static std::shared_ptr<T> make(Args &&... args)
		{
			return std::make_shared<T>(std::forward<Args>(args)...);
		}
	};
	using OptCall = OptCallHelper<tuple_type>;
};

template<class T, typename ... Types>
struct Factory: private FactoryImpl<T, Types...>::OptCall
{
	using base = typename FactoryImpl<T, Types...>::OptCall;
	using base::make;
};

/* Bind a native method:
 * 		`obj->*method(arg1, arg2, ...)`
 * to a methan0l method call:
 * 		`method(obj&, arg1, rg2, ...)`
 */
#define METHOD_WRAPPER(sig) \
		template<class C, typename R, typename ...Args> \
		constexpr auto method(sig) \
		{ \
			return [func](C& obj, Args ...args) -> R {return ((&obj)->*func)(args...);}; \
		}

METHOD_WRAPPER(R(C::*func)(Args...))
METHOD_WRAPPER(R(C::*func)(Args...)const)

#define CLASS(binder) decltype(binder)::bound_class
#define OBJECT(binder, obj) decltype(binder)::as_native(obj)

template<class C>
class ClassBinder
{
	private:
		using Obj = std::shared_ptr<C>;

		std::unique_ptr<Class> clazz;
		ExprEvaluator &eval;

	public:
		using bound_class = C;
		ClassBinder(const std::string &name, ExprEvaluator &eval) :
			clazz(std::make_unique<Class>(eval, name)), eval(eval) {}

		/*
		 * Creates a methan0l Class instance with specified constructor
		 *
		 * 	`C` - class being bound to a methan0l Class
		 * 	`Sig...` - `T`'s constructor signature
		 */
		template<typename ...Sig>
		inline void bind_constructor()
		{
			LOG("Wrapping ctor of " << type_name<C>() << " with sig: " << type_name<Sig...>())
			clazz->register_method(Class::CONSTRUCT, eval.bind_func([](Object &obj, Sig...args) {
				obj.def(mtl::str(Class::NATIVE_OBJ)) = Factory<C, Sig...>::make(std::forward<Sig>(args)...);
			}));
		}

		inline Class& get_class()
		{
			return *clazz;
		}

		static inline C& as_native(Object &obj)
		{
			return *obj.get_native().get<Obj>();
		}

		template<typename F>
		inline void bind_method(std::string_view name, F &&method)
		{
			clazz->register_method(name, eval.bind_func(mtl::method(method)));
		}

		template<typename F>
		inline void register_method(std::string_view name, F &&method)
		{
			clazz->register_method(name, eval.bind_func(method));
		}

		inline void register_class()
		{
			eval.get_type_mgr().register_type(std::move(clazz));
		}
};

struct BinderTest
{
	static void pair(ExprEvaluator&);
};

}

#endif /* SRC_UTIL_CLASS_BINDER_H_ */
