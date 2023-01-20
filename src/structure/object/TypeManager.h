#ifndef SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_
#define SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_

#include <string>
#include <unordered_map>

#include "util/meta/type_traits.h"
#include "util/memory.h"
#include "Object.h"

namespace mtl
{

class Anonymous;

class TypeManager
{
	private:
		std::pmr::unordered_map<size_t, std::shared_ptr<Class>> types;
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
			/* If T is callable, it must handle the registration itself inside its invocation operator overload
			 * (+ the registration must not have any state bound to object of T) */
			IF (is_callable<T>::value) {
				T registrator(context);
				registrator();
			}
			else
				register_type(Allocatable<T>::allocate(context));
		}

		void unregister_type(const std::string &name);

		Value invoke_method(Object &obj, const std::string &name, Args &args);

		Object create_object(const std::string &type_name, Args &args);
		Object create_object(size_t type_id, Args &args);

		Class* get_root();
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_ */
