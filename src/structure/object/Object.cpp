#include "Object.h"

#include <iomanip>
#include <deque>
#include <memory>
#include <string_view>
#include <sstream>
#include <variant>

#include "../../expression/LiteralExpr.h"
#include "../../lang/Library.h"
#include "../../type.h"
#include "../../util/util.h"
#include "TypeManager.h"

namespace mtl
{

Object::Object()
{
}

Object::Object(size_t type_hash) : type_hash(type_hash)
{
}

Object::Object(const Object &rhs) : type_hash(rhs.type_hash), data(rhs.data)
{
}

Object& Object::operator=(const Object &rhs)
{
	type_hash = rhs.type_hash;
	data = rhs.data;
	return *this;
}

Value& Object::field(const std::string &name)
{
	return data.get_or_create(name);
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
	args.push_front(std::make_shared<LiteralExpr>(*this));
}

Object& Object::get_this(ExprList &args)
{
	LiteralExpr &this_expr = try_cast<LiteralExpr>(args.front());
	return std::get<Object>(this_expr.raw_ref());
}

DataTable& Object::get_data()
{
	return data;
}

size_t Object::type_id() const
{
	return type_hash;
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
			<< " of Type 0x" << std::hex << obj.type_id() << "}";
}

Object Object::copy(const Object &obj)
{
	Object new_obj(obj);
	new_obj.deep_copy();
	return new_obj;
}

} /* namespace mtl */
