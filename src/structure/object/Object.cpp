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

Object::Object()
{
}

Object::Object(Class* type) : objclass(type)
{
}

Object::Object(Class* type, const DataTable &proto_data) :
		objclass(type),
		data(proto_data)
{
	deep_copy();
}

Object::Object(const Object &rhs) :
		objclass(rhs.objclass),
		this_instance(rhs.this_instance),
		data(rhs.data)
{
}

Object& Object::operator=(const Object &rhs)
{
	objclass = rhs.objclass;
	data = rhs.data;
	this_instance = rhs.this_instance;
	return *this;
}

Value& Object::field(const std::string &name)
{
	return prv_access ? data.get_or_create(name) : data.get(name, true);
}

Value& Object::field(const std::string_view &name)
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

void Object::inject_this(Args &args)
{
	args.push_front(this_instance);
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

bool Object::has_prv_access()
{
	return prv_access;
}

DataTable& Object::get_data()
{
	return data;
}

size_t Object::type_id() const
{
	return objclass->get_id();
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
	init_self(this_instance = std::make_shared<LiteralExpr>(*this));
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

void Object::init_self(std::shared_ptr<LiteralExpr> &this_instance)
{
	Object &ths = this_instance->raw_ref().get<Object>();
	ths.this_instance = this_instance;
	ths.prv_access = true;
}

Object Object::copy(const Object &obj)
{
	Object new_obj(obj);
	new_obj.deep_copy();
	return new_obj;
}

} /* namespace mtl */
