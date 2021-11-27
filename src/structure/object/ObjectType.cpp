#include "ObjectType.h"

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

ObjectType::ObjectType(ExprEvaluator &eval, const std::string &name) :
		id(get_id(name)),
		eval(eval)
{
	Object::init_self(static_instance = std::make_shared<LiteralExpr>(Object(id)));

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

void ObjectType::register_method(const std::string &name, Function method)
{
	method.arg_def.push_front(std::make_pair<std::string, ExprPtr>("this", LiteralExpr::empty()));
	class_data.set(name, Value(method));
}

Value ObjectType::invoke_method(Object &obj, const std::string &name, ExprList &args)
{
	DataTable &data = get_class_data();
	Value &field = data.get(name);

	if (field.nil())
		return Value::NO_VALUE;

	field.assert_type(Type::FUNCTION, "Trying to invoke a non-function");

	Function method = field.get<Function>();
	return eval.invoke(method, args);
}

Value ObjectType::invoke_static(const std::string &name, ExprList &args)
{
	auto arg_copy = args;
	arg_copy.push_front(static_instance);
	return invoke_method(static_instance->raw_ref().get<Object>(), name, arg_copy);
}

void ObjectType::register_private(std::string name)
{
	private_members.emplace(name);
}

bool ObjectType::is_private(const std::string &name)
{
	return private_members.find(name) != private_members.end();
}

DataTable& ObjectType::get_class_data()
{
	return class_data;
}

DataTable& ObjectType::get_object_data()
{
	return proto_object_data;
}

size_t ObjectType::get_id()
{
	return id;
}

Object ObjectType::create(Args &args)
{
	Object obj(id, proto_object_data);
	obj.invoke_method(eval.get_type_mgr(), CONSTRUCT, args);
	return obj;
}

size_t ObjectType::get_id(const std::string &type_name)
{
	return str_hash(type_name);
}

std::ostream& operator <<(std::ostream &stream, ObjectType &type)
{
	return stream << "{Type 0x" << std::hex << type.get_id() << std::dec << "}";
}

} /* namespace mtl */
