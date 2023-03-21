#ifndef SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_
#define SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_

#include <oop/Object.h>
#include <structure/TypeID.h>
#include <string>
#include <unordered_map>

#include "util/meta/type_traits.h"
#include "util/memory.h"
#include "util/hash.h"

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

		template<typename Type, typename ...Args>
		Object make_new_object(Args &&...ctor_args)
		{
			auto native_id = TypeID::of<Type>();
			auto entry = native_classes.find(native_id);
			if (entry == native_classes.end())
				throw std::runtime_error("Cannot instantiate an object "
						"of unregistered native type: " + str(mtl::type_name<Type>())
						+ " (" + mtl::str(native_id.type_name()) + ")");

			auto obj = create_uninitialized_object(entry->second);
			auto native_obj = std::allocate_shared<Type>(
					std::pmr::polymorphic_allocator<Type> {},
					std::forward<Args>(ctor_args)...);
			obj.set_native(native_obj);
			return obj;
		}

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
			return create_object(type_name, {ctor_args...});
		}

		void unregister_type(const std::string &name);

		Value invoke_method(Object &obj, const std::string &name, Args &args);

		Object create_object(const std::string &type_name, Args &args);
		Object create_object(size_t type_id, Args &args);
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
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_ */
