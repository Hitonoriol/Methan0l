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

#include "interpreter/ExprEvaluator.h"
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
		name(name),
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
	register_method("class_id", [&](Args &args) {
		return id;
	});

	/* Default constructor */
	register_method(CONSTRUCT, [&](Args &args) {
		return Value::NO_VALUE;
	});

	/* Default string conversion */
	register_method(TO_STRING, [&](Object &obj) {
		return obj.to_string();
	});

	/* Get all methods of this class */
	register_method("get_methods", [&](Object &obj) {
		return extract_names(class_data);
	});

	/* Get all fields of object of this class */
	register_method("get_fields", [&](Object &obj) {
		return extract_names(obj.get_data());
	});

	/* Get method by name */
	register_method("get_method", [&](Object &obj, const std::string &name) {
		return class_data.get(name, true);
	});
}

void Class::register_method(std::string_view name, Function &method)
{
	method.arg_def.push_front({std::move(mtl::str(Class::THIS_ARG)), LiteralExpr::empty()});
	class_data.set(mtl::str(name), method);
}

Value Class::extract_names(const DataTable &table)
{
	Value names(Type::LIST);
	auto &list = names.get<ValList>();
	for (auto&& [name, method] : table.managed_map()) {
		if (name[0] != '.')
			list.push_back(name);
	}
	return names;
}

Value Class::invoke_method(Object &obj, const std::string &name, ExprList &args)
{
	DataTable &data = get_class_data();
	return eval.invoke_method(obj, data.get(name, true), args);
}

Value Anonymous::invoke_method(Object &obj, const std::string &name, ExprList &args)
{
	return eval.invoke_method(obj, obj.field(name), args);
}

Value Class::invoke_static(const std::string &name, ExprList &args)
{
	return invoke_method(*static_instance, name, args);
}

bool Class::static_call(Args &args)
{
	return Object::get_this(args).id() == static_instance->id();
}

DataTable& Class::get_class_data()
{
	return class_data;
}

DataTable& Class::get_object_data()
{
	return proto_object_data;
}

const std::string& Class::get_name()
{
	return name;
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
