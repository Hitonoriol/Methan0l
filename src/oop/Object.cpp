#include "Object.h"

#include <iomanip>
#include <deque>
#include <memory>
#include <string_view>
#include <sstream>
#include <variant>

#include <expression/LiteralExpr.h>
#include <core/Library.h>
#include <util/util.h>
#include <interpreter/TypeManager.h>
#include <interpreter/Interpreter.h>
#include <oop/Class.h>
#include <CoreLibrary.h>

namespace mtl
{

const std::string OBJCLASS(".class");

Object::Object(Class* type)
	: data(&type->get_context())
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

Object& Object::construct(Args &args)
{
	invoke_method(str(Methods::Constructor), args);
	return *this;
}


Value Object::invoke_method(const std::string &name, Args &args)
{
	return get_class()->invoke_method(*this, name, args);
}

Value& Object::get_this_v(Args &args)
{
	LiteralExpr &this_expr = try_cast<LiteralExpr>(args.front());
	return this_expr.raw_ref();
}

std::any& Object::get_native_any()
{
	return field(Fields::NATIVE_OBJ).get<std::any>();
}

Value& Object::get_native()
{
	return field(Fields::NATIVE_OBJ);
}

bool Object::is_native()
{
	return data.exists(str(Fields::NATIVE_OBJ));
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
	return get_class()->get_context();
}

class_id Object::type_id() const
{
	return get_class()->get_id();
}

Int Object::id() const
{
	return reinterpret_cast<Int>(&(data.managed_map()));
}

Shared<native::String> Object::to_string()
{
	return invoke_method(str(Methods::ToString), empty_args).get<Shared<native::String>>();
}

Shared<native::String> Object::to_string_default()
{
	sstream ss;
	ss << *this;
	return context().make<String>(ss.str());
}

void Object::deep_copy()
{
	data.copy_managed_map();
}

bool operator ==(const Object &lhs, const Object &rhs)
{
	return unconst(lhs).invoke_method(mtl::str(Methods::Equals), LiteralExpr::make_list(unconst(rhs)));
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
