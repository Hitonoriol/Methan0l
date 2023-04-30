#ifndef SRC_STRUCTURE_OBJECT_H_
#define SRC_STRUCTURE_OBJECT_H_

#include "OOPDefs.h"

#include <string>
#include <any>

#include <structure/DataTable.h>
#include <util/cast.h>

namespace mtl
{

class TypeManager;
class Function;
class Interpreter;
class LiteralExpr;
class Class;

namespace native
{

class String;

}

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

		Object& construct(Args &args);
		Value& field(std::string_view name);
		Value& field(const std::string &name);
		Value& def(std::string_view name);
		Value invoke_method(const std::string &name, Args &args = empty_args);

		static Value& get_this_v(Args &args);
		static Object& get_this(Args &args);

		std::any& get_native_any();
		Value& get_native();
		void set_native(Value);

		Class* get_class() const;
		Interpreter& context() const;

		DataTable& get_data();
		class_id type_id() const;
		Int id() const;

		Shared<native::String> to_string();
		Shared<native::String> to_string_default();

		friend bool operator ==(const Object &lhs, const Object &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Object &obj);

		static void init_self(std::shared_ptr<LiteralExpr> &this_instance);
		static Object copy(const Object &obj);

		template<typename T>
		static inline Object copy_native(Object &obj, Allocator<T> alloc = {})
		{
			auto new_obj = copy(obj);
			auto &original_native = *mtl::any_cast<std::shared_ptr<T>&>(obj.get_native_any());
			auto new_native = std::allocate_shared<T>(alloc, original_native);
			new_obj.set_native(new_native);
			return new_obj;
		}
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_H_ */
