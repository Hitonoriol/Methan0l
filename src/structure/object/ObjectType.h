#ifndef SRC_STRUCTURE_OBJECT_OBJECTTYPE_H_
#define SRC_STRUCTURE_OBJECT_OBJECTTYPE_H_

#include <crtdefs.h>
#include <string>
#include <unordered_map>

#include "../../type.h"
#include "../DataTable.h"

namespace mtl
{

class Function;
class ExprEvaluator;

template<typename T>
using Managed = std::unordered_map<uintptr_t, T>;

class Object;

class ObjectType
{
	private:
		size_t id;

		/* Methods & fields that are associated with this Type */
		DataTable class_data;

		/* Data that will be copied into every Object of this Type upon creation */
		DataTable proto_object_data;

	protected:
		ExprEvaluator &eval;

	public:
		static constexpr std::string_view
		CONSTRUCT = "construct", TO_STRING = "to_string";

		ObjectType(ExprEvaluator &eval, const std::string &name);
		virtual ~ObjectType() = default;

		void register_method(std::string &name, Function method);

		DataTable& get_class_data();
		DataTable& get_object_data();

		virtual Value invoke_method(Object &obj, const std::string &name, ExprList &args);

		size_t get_id();
		virtual Object create(ExprList &args);

		friend std::ostream& operator <<(std::ostream &stream, ObjectType &type);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_OBJECTTYPE_H_ */
