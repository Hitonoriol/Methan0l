#include "Class.h"

#include <array>
#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <variant>

#include <util/hash.h>
#include <oop/Object.h>
#include <lang/Library.h>
#include <expression/LiteralExpr.h>
#include <structure/Function.h>
#include <structure/Value.h>
#include <interpreter/Interpreter.h>
#include <CoreLibrary.h>

namespace mtl
{

Class::Class(Interpreter &context, const std::string &name) :
		static_instance(std::make_unique<Object>(this)),
		context(context)
{
	set_name(name);

	/* Default constructor */
	register_method(Methods::Constructor, [&](Args &args) {
		return Value::NO_VALUE;
	});

	/* Default equals comparison: compare object memory addresses */
	register_method(Methods::Equals, [&](OBJ, Object &rhs) {
		return this_obj.id() == rhs.id();
	});

	/* Default string conversion */
	register_method(Methods::ToString, [&](Object &obj) {
		return obj.to_string_default();
	});

	/* Default object copy */
	register_method(Methods::Copy, [&](Object &obj) {
		return Object::copy(obj);
	});

	/* Default object hash_code */
	register_method(Methods::Hashcode, [&](Object &obj) {
		return obj.id();
	});

	/* Reflective methods: */

	/* [Static] Get class id */
	register_method("class_id", [&](Args &args) {
		return get_id();
	});

	/* [Static] Get all methods of this class */
	register_method("get_methods", [&](Object &obj) {
		return get_methods();
	});

	/* [Non-static] Get all fields of object of this class */
	register_method("get_fields", [&](Object &obj) {
		return get_fields(obj);
	});

	/* [Static] Get method by name */
	register_method("get_method", [&](Object &obj, const std::string &name) {
		return class_data.get(name, true);
	});

	/* [Non-static] Get field value by name */
	register_method("get_field", [&](Object &obj, const std::string &name) {
		return obj.field(name);
	});

	register_method("is_native", [&] {
		return is_native();
	});

	register_method("get_native", [&](Object &obj) {
		return obj.get_native();
	});
}

void Class::register_method(std::string_view name, Function &method)
{
	name = mtl::strip_name_scope(name);
	method.arg_def.push_front( { std::move(mtl::str(Parameters::This)),
			LiteralExpr::empty() });
	class_data.set(mtl::str(name), method);
}

void Class::add_base_class(Class *base_class)
{
	for (auto& [name, value] : *(base_class->get_class_data().map_ptr())) {
		if (name != Methods::Constructor)
			class_data.set(name, value);
	}
}

void Class::inherit(Class *parent)
{
	if (has_superclass())
		throw std::runtime_error("Classes can only have one superclass");

	superclass = parent;
	for (auto& [name, value] : *(parent->get_object_data().map_ptr()))
		proto_object_data.set(name, value);
	class_data.set(str(Parameters::Super), parent->get_class_data().get(str(Methods::Constructor)));
	add_base_class(parent);

}

void Class::implement(Class *interface)
{
	if (!interface->proto_object_data.empty())
		throw std::runtime_error(get_name() + "is unable to implement "
			"a non-interface: " + interface->get_name());

	interfaces.push_back(interface);
	add_base_class(interface);
}

const std::vector<Class*>& Class::get_interfaces()
{
	return interfaces;
}

Value Class::get_methods()
{
	return extract_names(class_data);
}

Value Class::get_fields(Object &obj)
{
	return extract_names(obj.get_data());
}

Value Class::get_fields()
{
	return extract_names(proto_object_data);
}

Value Class::extract_names(const DataTable &table)
{
	auto names = context.make<List>();
	auto &list = names.get<List>();
	for (auto&& [name, method] : table.managed_map()) {
		if (name[0] != '.')
			list->push_back(name);
	}
	return names;
}

Value Class::invoke_method(Object &obj, const std::string &name, Args &args)
{
	DataTable &data = get_class_data();
	return context.invoke_method(obj, data.get(name, true), args);
}

Value Anonymous::invoke_method(Object &obj, const std::string &name, Args &args)
{
	return context.invoke_method(obj, obj.field(name), args);
}

Value Class::invoke_static(const std::string &name, Args &args)
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

Value Class::get_method(const std::string &name)
{
	return class_data.get(name, true);
}

const std::string& Class::get_name()
{
	return name;
}

Object Class::create(Args &args)
{
	auto obj = create_uninitialized();
	return obj.construct(args);
}

Object Class::create_uninitialized()
{
	return Object(this, proto_object_data);
}

std::ostream& operator <<(std::ostream &stream, Class &type)
{
	return stream << "{"
			<< (type.native ? "Native" : "Runtime")
			<< " Class "
			<< std::quoted(type.name)
			<< " (" << std::hex << type.get_id() << std::dec << ")}";
}

} /* namespace mtl */
