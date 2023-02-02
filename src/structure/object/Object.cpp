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
#include "interpreter/Interpreter.h"
#include "structure/object/Class.h"

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

Object& Object::construct(TypeManager &mgr, Args &args)
{
	invoke_method(Methods::CONSTRUCTOR, args);
	return *this;
}


Value Object::invoke_method(const std::string &name, Args &args)
{
	return get_class()->invoke_method(*this, name, args);
}

Value Object::invoke_method(const std::string_view &name, Args &args)
{
	return invoke_method(str(name), args);
}

Value& Object::get_this_v(Args &args)
{
	LiteralExpr &this_expr = try_cast<LiteralExpr>(args.front());
	return this_expr.raw_ref();
}

Value& Object::get_native()
{
	return field(Fields::NATIVE_OBJ);
}

void Object::set_native(Value native_obj)
{
	def(Fields::NATIVE_OBJ) = native_obj;
}

Object& Object::get_this(Args &args)
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

Interpreter& Object::context() const
{
	return get_class()->get_evatuator();
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
	Args noargs;
	return str(invoke_method(Methods::TO_STRING, noargs));
}

std::string Object::to_string_default()
{
	sstream ss;
	ss << *this;
	return ss.str();
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
	return stream << "{Object " << obj.get_data().map_ptr()
			<< " of class `" << obj.get_class()->get_name() << "`}";
}

Object Object::copy(const Object &obj)
{
	Object new_obj(obj);
	new_obj.deep_copy();
	return new_obj;
}

} /* namespace mtl */
