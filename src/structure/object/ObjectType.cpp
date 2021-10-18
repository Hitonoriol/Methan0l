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

#include "../../expression/LiteralExpr.h"
#include "../../ExprEvaluator.h"
#include "../../lang/Library.h"
#include "../Function.h"
#include "../Value.h"
#include "Object.h"

namespace mtl
{

ObjectType::ObjectType(ExprEvaluator &eval, const std::string &name) :
		id(get_id(name)),
		eval(eval)
{
	static_instance = std::make_shared<LiteralExpr>(Object(id));
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
		return NO_VALUE;

	if (field.type != Type::FUNCTION)
		throw std::runtime_error("Trying to invoke a non-function");

	Function &method = field.get<Function>();
	return eval.invoke(method, args);
}

Value ObjectType::invoke_static(const std::string &name, ExprList &args)
{
	return invoke_method(std::get<Object>(static_instance->raw_ref()), name, args);
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
	return std::hash<std::string> { }(type_name);
}

std::ostream& operator <<(std::ostream &stream, ObjectType &type)
{
	return stream << "{Type 0x" << std::hex << type.get_id() << std::dec << "}";
}

} /* namespace mtl */
