#ifndef SRC_STRUCTURE_OBJECT_OBJECTTYPE_H_
#define SRC_STRUCTURE_OBJECT_OBJECTTYPE_H_

#include <string>
#include <set>
#include <unordered_map>


#include "../../type.h"
#include "../DataTable.h"

namespace mtl
{

class LiteralExpr;
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
		std::set<std::string> private_members;

		/* Data that will be copied into every Object of this Type upon creation */
		DataTable proto_object_data;

		std::shared_ptr<LiteralExpr> static_instance;

	protected:
		ExprEvaluator &eval;

	public:
		static constexpr std::string_view
		CONSTRUCT = "construct", TO_STRING = "to_string";

		ObjectType(ExprEvaluator &eval, const std::string &name);
		virtual ~ObjectType() = default;

		void register_method(const std::string &name, Function method);

		DataTable& get_class_data();
		DataTable& get_object_data();

		void register_private(std::string name);
		bool is_private(const std::string &name);

		bool static_call(Args &args);

		virtual Value invoke_method(Object &obj, const std::string &name, ExprList &args);
		Value invoke_static(const std::string &name, ExprList &args);

		size_t get_id();
		virtual Object create(ExprList &args);

		static size_t get_id(const std::string &type_name);

		friend std::ostream& operator <<(std::ostream &stream, ObjectType &type);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_OBJECTTYPE_H_ */
