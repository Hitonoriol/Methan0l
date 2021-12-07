#ifndef SRC_STRUCTURE_OBJECT_H_
#define SRC_STRUCTURE_OBJECT_H_

#include <string>

#include "../DataTable.h"

namespace mtl
{

class TypeManager;
class Function;
class ExprEvaluator;
class LiteralExpr;

class Object
{
	private:
		friend class ObjectType;

		size_t type_hash;
		bool prv_access = false;
		std::shared_ptr<LiteralExpr> this_instance = nullptr;

		void deep_copy();

	protected:
		DataTable data;

	public:
		Object();
		Object(size_t type_hash);
		Object(size_t type_hash, const DataTable &proto_data);
		Object(const Object &rhs);
		Object& operator=(const Object &rhs);

		Value& field(const std::string_view &name);
		Value& field(const std::string &name);
		Value invoke_method(TypeManager &mgr, const std::string &name, ExprList &args);
		Value invoke_method(TypeManager &mgr, const std::string_view &name,
				ExprList &args);

		void inject_this(ExprList &args);

		static Value& get_this_v(ExprList &args);
		static Object& get_this(ExprList &args);

		bool has_prv_access();
		DataTable& get_data();
		size_t type_id() const;
		uintptr_t id() const;

		std::string to_string();
		std::string to_string(ExprEvaluator &eval);

		friend bool operator ==(const Object &lhs, const Object &rhs);
		friend std::ostream& operator <<(std::ostream &stream, Object &obj);

		static void init_self(std::shared_ptr<LiteralExpr> &this_instance);
		static Object copy(const Object &obj);
};

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_H_ */
