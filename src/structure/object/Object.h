#ifndef SRC_STRUCTURE_OBJECT_H_
#define SRC_STRUCTURE_OBJECT_H_

#include <crtdefs.h>
#include <string>

#include "../DataTable.h"

namespace mtl
{

class TypeManager;
class Function;

class Object
{
	private:
		size_t type_hash;

		void deep_copy();
		static constexpr std::string_view EQUALS = "equals";

	protected:
		DataTable data;

	public:
		Object();
		Object(size_t type_hash);
		Object(const Object &rhs);
		Object& operator=(const Object &rhs);

		Value& field(const std::string_view &name);
		Value& field(const std::string &name);
		Value invoke_method(TypeManager &mgr, const std::string &name, ExprList &args);
		Value invoke_method(TypeManager &mgr, const std::string_view &name, ExprList &args);

		void inject_this(ExprList &args);
		static Object& get_this(ExprList &args);

		DataTable& get_data();
		size_t type_id() const;
		uintptr_t id() const;
		std::string to_string();

		friend bool operator ==(const Object &lhs, const Object &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Object &obj);

		static Object copy(const Object &obj);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_H_ */
