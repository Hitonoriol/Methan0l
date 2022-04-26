#ifndef SRC_UTIL_CLASS_BINDER_H_
#define SRC_UTIL_CLASS_BINDER_H_

#include "ExprEvaluator.h"
#include "structure/object/InbuiltClass.h"

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

template<class C>
class ClassBinder
{
	private:
		using Obj = std::shared_ptr<C>;

		std::unique_ptr<InbuiltClass> clazz;
		ExprEvaluator &eval;

	public:
		ClassBinder(const std::string &name, ExprEvaluator &eval) :
			clazz(std::make_unique<InbuiltClass>(eval, name)), eval(eval) {}

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

		inline InbuiltClass& get_class()
		{
			return *clazz;
		}

		static inline C& as_native(Object &obj)
		{
			return *obj.field(Class::NATIVE_OBJ).get<Obj>();
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
