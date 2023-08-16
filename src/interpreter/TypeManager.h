#ifndef SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_
#define SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_

#include <string>
#include <unordered_map>

#include <util/meta/type_traits.h>
#include <util/memory.h>
#include <util/hash.h>
#include <oop/Object.h>
#include <structure/TypeID.h>
#include <expression/ListExpr.h>

namespace mtl
{

class Anonymous;

class TypeManager
{
	public:
		using IdIndex = HashMap<class_id, std::shared_ptr<Class>>;
		using NameIndex = HashMap<std::string, Class*>;
		using NativeIndex = HashMap<TypeID, Class*>;

	private:
		IdIndex classes;
		NameIndex class_index;
		NativeIndex native_classes;
		Interpreter &context;
		std::shared_ptr<Anonymous> root;

	public:
		TypeManager(Interpreter &context);
		~TypeManager();

		void register_type(std::shared_ptr<Class> type);

		const TypeID& get_type(Int id);
		const TypeID& get_type(const std::string& name);

		Class& get_class(Int id);
		Class& get_class(const std::string& name);

		template<typename T>
		inline void register_type()
		{
			/*   If T is callable, it must handle the registration itself inside its invocation operator overload
			 * (+ the registration must not have any state bound to object of T).
			 *   This mechanism is used by NativeClass<T>. */
			IF (is_callable<T>::value) {
				T registrator(context);
				registrator();
			}
			else /* Allocate a mtl::Class instance and register it - for classes not backed by native classes */
				register_type(Allocatable<T>::allocate(context));
		}

		/*   Allocate a new object of a native-backed methan0l type.
		 * `Binding` must inherit from NativeClass<T>, where T is the
		 * native class bound to a mtl::Class.
		 *   Class `Binding` must be registered via the register_type<T>() method above. */
		template<class Binding, typename BoundClass = typename Binding::bound_class, typename ...Args>
		inline Object new_object(Args &&...ctor_args)
		{
			return make_new_object<BoundClass>(std::forward<Args>(ctor_args)...);
		}

		/*   Allocate a new object of a native-backed methan0l type.
		 * `Type` is a type that must've been registered via a
		 * NativeClass<Type> class binding before calling this. */
		template<typename Type, typename ...Args>
		inline auto new_object(Args &&...ctor_args) -> std::enable_if_t<!is_class_binding<Type>::value, Object>
		{
			return make_new_object<Type>(std::forward<Args>(ctor_args)...);
		}

		template<typename ...Args>
		inline Object new_object(const std::string &type_name, Args &&...ctor_args)
		{
			return create_object(type_name, {Value::wrapped(ctor_args)...});
		}

		template<typename ...Args>
		inline Object new_object(TypeID type_id, Args &&...ctor_args)
		{
			return create_object(type_id.type_id(), {Value::wrapped(ctor_args)...});
		}

		inline Object bind_object(TypeID class_id, const Value &raw_obj)
		{
			auto it = native_classes.find(class_id);
			if (it == native_classes.end())
				throw std::runtime_error("Unable to bind raw object of type "
						+ mtl::str(raw_obj.type().type_name())
						+ " to class " + mtl::str(class_id.type_name()));

			auto obj = create_uninitialized_object(it->second);
			obj.set_native(raw_obj);
			return obj;
		}

		/* Bind an existing raw native object to its runtime Methan0l class */
		template<class C>
		inline Object bind_object(const Shared<C> &raw_obj)
		{
			return bind_object(TypeID::of<C>(), raw_obj);
		}

		void unregister_type(const std::string &name);

		Value invoke_method(Object &obj, const std::string &name, Args &args);

		Object create_object(const std::string &type_name, Args &args);
		Object create_object(class_id type_id, Args &args);
		Object create_uninitialized_object(Class*);

		Class* get_root();

		auto get_classes()
		{
			std::vector<Class*> classes;
			classes.reserve(class_index.size());
			for (auto &&[name, clazz] : class_index)
				classes.push_back(clazz);
			return classes;
		}

	private:
		template<typename Type, typename ...Args>
		Object make_new_object(Args &&...ctor_args)
		{
			auto native_id = TypeID::of<Type>();
			auto entry = native_classes.find(native_id);
			if (entry == native_classes.end()) {
				// Create a non-native class object
				if (classes.find(native_id.type_id()) != classes.end())
					return new_object(native_id, std::forward<Args>(ctor_args)...);

				throw std::runtime_error("Cannot instantiate an object "
						"of unregistered type: " + str(mtl::type_name<Type>())
						+ " (" + mtl::str(native_id.type_name()) + ")");
			}

			// Create a native class object
			IF (std::is_constructible_v<Type, Args...>) {
				auto obj = create_uninitialized_object(entry->second);
				auto native_obj = std::allocate_shared<Type>(
						std::pmr::polymorphic_allocator<Type> { },
						std::forward<Args>(ctor_args)...);
				obj.set_native(native_obj);
				return obj;
			} else {
				// Unreachable
				throw std::runtime_error("TypeManager::make_new_object");
			}
		}
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_ */
