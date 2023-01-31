#ifndef SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_
#define SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_

#include <string>
#include <unordered_map>
#include <typeinfo>

#include "util/meta/type_traits.h"
#include "util/memory.h"
#include "util/hash.h"
#include "Object.h"

namespace mtl
{

class NativeID
{
	private:
		const std::type_info *type_info;

	public:
		NativeID(const std::type_info &type_info)
			: type_info(&type_info)	{}

		inline const std::type_info *operator->() const
		{
			return type_info;
		}

		inline bool operator==(const NativeID &rhs) const
		{
			return *type_info == *rhs.type_info;
		}

		template<typename C>
		static inline NativeID of()
		{
			return { typeid(C) };
		}
};

}

HASH(mtl::NativeID, v, {
	return v->hash_code();
})

namespace mtl
{

class Anonymous;

class TypeManager
{
	private:
		std::pmr::unordered_map<class_id, std::shared_ptr<Class>> types;
		std::pmr::unordered_map<NativeID, Class*> native_types;
		Interpreter &context;
		std::shared_ptr<Anonymous> root;

	public:
		TypeManager(Interpreter &context);
		~TypeManager();

		void register_type(std::shared_ptr<Class> type);

		Class& get_type(size_t id);
		Class& get_type(const std::string &name);

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

		/*   Allocate a new object of a native-backed methan0l type (C must inherit from NativeClass<T>),
		 * where T is the native class bound to a mtl::Class.
		 * Class C must be registered via the register_type<C>() method above. */
		template<class C, typename ...Args>
		Object new_object(Args &&...ctor_args)
		{
			using type = typename C::bound_class;
			auto native_id = NativeID::of<C>();
			auto entry = native_types.find(native_id);
			if (entry == native_types.end())
				throw std::runtime_error("Cannot instantiate an object "
						"of unregistered native type: " + str(mtl::type_name<type>())
						+ " (" + native_id->name() + ")");

			auto obj = create_uninitialized_object(entry->second);
			auto native_obj = std::allocate_shared<type>(std::pmr::polymorphic_allocator<type>{},
					std::forward<Args>(ctor_args)...);
			obj.set_native(native_obj);
			return obj;
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
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_ */
