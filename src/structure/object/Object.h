#ifndef SRC_STRUCTURE_OBJECT_H_
#define SRC_STRUCTURE_OBJECT_H_

#include <string>

#include "OOPDefs.h"
#include "structure/DataTable.h"

namespace mtl
{

class TypeManager;
class Function;
class Interpreter;
class LiteralExpr;
class Class;

class Object
{
	private:
		friend class Class;

		void deep_copy();

	protected:
		DataTable data;

	public:
		Object();
		Object(Class*);
		Object(Class*, const DataTable &proto_data);
		Object(const Object &rhs);
		Object& operator=(const Object &rhs);

		Object& construct(TypeManager &mgr, Args &args);
		Value& field(std::string_view name);
		Value& field(const std::string &name);
		Value& def(std::string_view name);
		Value invoke_method(const std::string &name, Args &args);
		Value invoke_method(const std::string_view &name, Args &args);

		static Value& get_this_v(Args &args);
		static Object& get_this(Args &args);

		Value& get_native();
		void set_native(Value);

		Class* get_class() const;

		DataTable& get_data();
		size_t type_id() const;
		uintptr_t id() const;

		std::string to_string();

		friend bool operator ==(const Object &lhs, const Object &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Object &obj);

		static void init_self(std::shared_ptr<LiteralExpr> &this_instance);
		static Object copy(const Object &obj);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_H_ */
