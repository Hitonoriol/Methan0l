#include <structure/object/Class.h>
#include <array>
#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <variant>

#include "ExprEvaluator.h"
#include "Object.h"
#include "lang/Library.h"
#include "expression/LiteralExpr.h"
#include "structure/Function.h"
#include "structure/Value.h"
#include "util/hash.h"

namespace mtl
{

Class::Class(ExprEvaluator &eval, const std::string &name) :
		id(get_id(name)),
		eval(eval)
{
	Object::init_self(static_instance = std::make_shared<LiteralExpr>(Object(this)));

	/*
	 * Can be called as a static method:
	 * 		Class@class_id$()
	 *
	 * 	As well as a method of an Object:
	 * 		obj.class_id$()
	 */
	register_method("class_id", Function::create( {
			Expression::return_val((dec) id)
	}));
}

void Class::register_method(const std::string &name, Function method)
{
	method.arg_def.push_front(std::make_pair<std::string, ExprPtr>("this", LiteralExpr::empty()));
	class_data.set(name, Value(method));
}

Value Class::invoke_method(Object &obj, const std::string &name, ExprList &args)
{
	DataTable &data = get_class_data();
	Value &field = data.get(name);

	if (field.nil())
		return Value::NO_VALUE;

	field.assert_type(Type::FUNCTION, "Trying to invoke a non-function");

	Function method = field.get<Function>();
	return eval.invoke(method, args);
}

Value Class::invoke_static(const std::string &name, ExprList &args)
{
	auto arg_copy = args;
	arg_copy.push_front(static_instance);
	return invoke_method(static_instance->raw_ref().get<Object>(), name, arg_copy);
}

void Class::register_private(std::string name)
{
	private_members.emplace(name);
}

bool Class::is_private(const std::string &name)
{
	return private_members.find(name) != private_members.end();
}

bool Class::static_call(Args &args)
{
	return static_cast<void*>(&Object::get_this(args)) == static_instance->raw_ref().identity();
}

DataTable& Class::get_class_data()
{
	return class_data;
}

DataTable& Class::get_object_data()
{
	return proto_object_data;
}

size_t Class::get_id()
{
	return id;
}

Object Class::create(Args &args)
{
	Object obj(this, proto_object_data);
	obj.invoke_method(eval.get_type_mgr(), CONSTRUCT, args);
	return obj;
}

size_t Class::get_id(const std::string &type_name)
{
	return str_hash(type_name);
}

std::ostream& operator <<(std::ostream &stream, Class &type)
{
	return stream << "{Type 0x" << std::hex << type.get_id() << std::dec << "}";
}

} /* namespace mtl */
