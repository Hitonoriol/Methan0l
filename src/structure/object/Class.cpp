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
		static_instance(std::make_unique<Object>(this)),
		eval(eval)
{
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
	class_data.set(name, method);
}

Value Class::invoke_method(Object &obj, const std::string &name, ExprList &args)
{
	DataTable &data = get_class_data();
	Value &field = data.get(name);

	if (field.nil())
		return Value::NO_VALUE;

	field.assert_type(Type::FUNCTION, "Trying to invoke a non-function");

	auto &method = eval.tmp_callable(field.get<Function>());
	method.call(eval, args);
	method.local().set(mtl::str(THIS_ARG), obj);
	return eval.execute(method);
}

Value Class::invoke_static(const std::string &name, ExprList &args)
{
	return invoke_method(*static_instance, name, args);
}

bool Class::static_call(Args &args)
{
	return &Object::get_this(args) == static_instance.get();
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
