#ifndef SRC_UTIL_CLASS_BINDER_H_
#define SRC_UTIL_CLASS_BINDER_H_

#include "interpreter/Interpreter.h"
#include "oop/Class.h"

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
			return [func](C& obj, Args ...args) -> R { \
				return ((&obj)->*func)(args...); \
			}; \
		} \
		template<class C, typename R, typename ...Args> \
		constexpr auto mutator_method(sig) \
		{ \
			return [func](const mtl::Value& obj, Args ...args) -> mtl::Value { \
				((&(mtl::unconst(obj).get<C>()))->*func)(args...); \
				return obj;\
			}; \
		}

METHOD_WRAPPER(R(C::*func)(Args...))
METHOD_WRAPPER(R(C::*func)(Args...)const)

/* Convenience macro to refer to the bound class of `binder` */
#define CLASS(binder) std::remove_reference<decltype(binder)>::type::bound_class

/* Convenience macro to get a reference to an object of `binder`'s bound class from the mtl::Object `obj` */
#define OBJECT(binder, obj) std::remove_reference<decltype(binder)>::type::as_native(obj)

/* Provides mechanisms for binding a native C++ class' constructor and methods to a mtl::Class instance. */
template<class C>
class ClassBinder
{
	private:
		using Obj = std::shared_ptr<C>;

		std::shared_ptr<Class> clazz;
		Interpreter &context;

	public:
		using bound_class = C;
		ClassBinder(Interpreter &context, const std::string &name = "") :
			clazz(Allocatable<Class>::allocate(context, name)), context(context) {}

		inline void set_name(std::string_view name)
		{
			clazz->set_name(name);
		}

		/* Binds the specified constructor of `bound_class` by signature to a methan0l class.
		 * Methan0l classes can only have one constructor.
		 * `Sig...` - `bound_class`'s constructor signature */
		template<typename ...Sig>
		inline void bind_constructor()
		{
			LOG("Wrapping ctor of " << type_name<C>() << " with sig: " << type_name<Sig...>())
			clazz->register_method(Methods::Constructor, context.bind_func([](Object &obj, Sig...args) {
				obj.def(mtl::str(Fields::NATIVE_OBJ)) = Factory<C, Sig...>::make(std::forward<Sig>(args)...);
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

		template<typename F, typename ...Types>
		inline void bind_method(std::string_view name, F &&method, Types &&...default_args)
		{
			clazz->register_method(name,
					context.bind_func(mtl::method(method),
							mtl::tuple(std::forward<Types>(default_args)...)));
		}

		template<typename F>
		inline void bind_method(std::string_view name, F &&method)
		{
			clazz->register_method(name, context.bind_func(mtl::method(method)));
		}

		template<typename F>
		inline void bind_mutator_method(std::string_view name, F &&method)
		{
			clazz->register_method(name, context.bind_func(mtl::mutator_method(method)));
		}

		template<typename F>
		inline void register_method(std::string_view name, F &&method)
		{
			LOG("Registering class method: " << name << "[" << type_name<F>() << "]")
			clazz->register_method(name, context.bind_func(method));
		}

		inline Class::MethodBinder register_method(std::string_view name)
		{
			return clazz->register_method(name);
		}

		inline void register_class()
		{
			context.get_type_mgr().register_type(clazz);
		}
};

}

#endif /* SRC_UTIL_CLASS_BINDER_H_ */
