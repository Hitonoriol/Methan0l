#include "Object.h"

#include <iomanip>
#include <deque>
#include <memory>
#include <string_view>
#include <sstream>
#include <variant>

#include "expression/LiteralExpr.h"
#include "lang/Library.h"
#include "type.h"
#include "util/util.h"
#include "TypeManager.h"
#include "ExprEvaluator.h"

namespace mtl
{

const std::string OBJCLASS(".class");

Object::Object()
{
}

Object::Object(Class* type)
{
	def(OBJCLASS) = type;
}

Object::Object(Class* type, const DataTable &proto_data) :
		data(proto_data)
{
	deep_copy();
	def(OBJCLASS) = type;
}

Object::Object(const Object &rhs) :
		data(rhs.data)
{
}

Object& Object::operator=(const Object &rhs)
{
	data = rhs.data;
	return *this;
}

Value& Object::field(const std::string &name)
{
	return data.get(name, true);
}

Value& Object::def(std::string_view name)
{
	return data.get_or_create(mtl::str(name));
}

Value& Object::field(std::string_view name)
{
	return field(std::string(name));
}

Value Object::invoke_method(TypeManager &mgr, const std::string &name, ExprList &args)
{
	return mgr.invoke_method(*this, name, args);
}

Value Object::invoke_method(TypeManager &mgr, const std::string_view &name,
		ExprList &args)
{
	return invoke_method(mgr, std::string(name), args);
}

Value& Object::get_this_v(ExprList &args)
{
	LiteralExpr &this_expr = try_cast<LiteralExpr>(args.front());
	return this_expr.raw_ref();
}

Object& Object::get_this(ExprList &args)
{
	return get_this_v(args).get<Object>();
}

DataTable& Object::get_data()
{
	return data;
}

Class* Object::get_class() const
{
	return data.cget(OBJCLASS);
}

size_t Object::type_id() const
{
	return get_class()->get_id();
}

uintptr_t Object::id() const
{
	return reinterpret_cast<uintptr_t>(&(data.managed_map()));
}

std::string Object::to_string()
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

std::string Object::to_string(ExprEvaluator &eval)
{
	ExprList noargs;
	return str(invoke_method(eval.get_type_mgr(), Class::TO_STRING, noargs));
}

void Object::deep_copy()
{
	data.copy_managed_map();
}

bool operator ==(const Object &lhs, const Object &rhs)
{
	return &lhs == &rhs;
}

std::ostream& operator <<(std::ostream &stream, Object &obj)
{
	return stream << "{Object 0x" << std::hex << obj.id()
			<< " of Type 0x" << std::hex << obj.type_id()
			<< std::dec << "}";
}

Object Object::copy(const Object &obj)
{
	Object new_obj(obj);
	new_obj.deep_copy();
	return new_obj;
}

} /* namespace mtl */
