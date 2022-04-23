#ifndef SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_
#define SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_

#include <structure/object/Class.h>
#include <string>
#include <unordered_map>

#include "../../lang/Library.h"
#include "Object.h"

namespace mtl
{

class TypeManager
{
	private:
		std::unordered_map<size_t, std::unique_ptr<Class>> types;
		ExprEvaluator &eval;

	public:
		TypeManager(ExprEvaluator &eval) : eval(eval) {}
		Class& get_type(size_t id);
		void register_type(std::unique_ptr<Class> &&type);

		template<typename T>
		inline void register_type()
		{
			register_type(std::make_unique<T>(eval));
		}

		Value invoke_method(Object &obj, const std::string &name, Args &args);

		Object create_object(const std::string &type_name, Args &args);
		Object create_object(size_t type_id, Args &args);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_TYPEMANAGER_H_ */
