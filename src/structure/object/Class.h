#ifndef SRC_STRUCTURE_OBJECT_CLASS_H_
#define SRC_STRUCTURE_OBJECT_CLASS_H_

#include <string>
#include <set>
#include <unordered_map>


#include "type.h"
#include "structure/DataTable.h"

namespace mtl
{

class LiteralExpr;
class Function;
class ExprEvaluator;

template<typename T>
using Managed = std::unordered_map<uintptr_t, T>;

class Object;

class Class
{
	private:
		size_t id;

		/* Methods & fields that are associated with this Class */
		DataTable class_data;

		/* Data that will be copied into every Object of this Class upon creation */
		DataTable proto_object_data;

		std::unique_ptr<Object> static_instance;

	protected:
		ExprEvaluator &eval;

	public:
		static constexpr std::string_view
		CONSTRUCT = "construct", TO_STRING = "to_string", THIS_ARG = "this",
		NATIVE_OBJ = ".o";

		Class(ExprEvaluator &eval, const std::string &name);
		virtual ~Class() = default;

		void register_method(const std::string&, Function);

		DataTable& get_class_data();
		DataTable& get_object_data();

		bool static_call(Args &args);

		virtual Value invoke_method(Object &obj, const std::string &name, ExprList &args);
		Value invoke_static(const std::string &name, ExprList &args);

		size_t get_id();
		virtual Object create(ExprList &args);

		static size_t get_id(const std::string &type_name);

		friend std::ostream& operator <<(std::ostream &stream, Class &type);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_CLASS_H_ */
