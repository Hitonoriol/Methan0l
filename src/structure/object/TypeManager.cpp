#include "TypeManager.h"

#include <iomanip>
#include <stdexcept>
#include <utility>

#include "expression/LiteralExpr.h"
#include "interpreter/Interpreter.h"
#include "structure/DataTable.h"
#include "structure/Function.h"
#include "structure/Value.h"

#include "structure/object/Class.h"

namespace mtl
{

TypeManager::TypeManager(Interpreter &context)
		: context(context), root(std::make_shared<Anonymous>(context))
{}

TypeManager::~TypeManager() = default;

void TypeManager::register_type(std::shared_ptr<Class> type)
{
	LOG("! Registering type ["
			<< std::hex << type->get_id() << "] " << std::dec
			<< type->get_class_data())

	auto type_id = type->get_id();
	auto native_id = type->get_native_id();
	classes.emplace(type_id, type);
	if (native_id != nullptr) {
		LOG("Registered a new native type: " << native_id->name())
		native_classes.emplace(*native_id, type.get());
	}
}

void TypeManager::unregister_type(const std::string &name)
{
	classes.erase(Class::get_id(name));
}

Class& TypeManager::get_type(size_t id)
{
	auto entry = classes.find(id);
	if (entry != classes.end())
		return *(entry->second);

	if (id == root->get_id())
		return *root;

	throw std::runtime_error("Trying to resolve a non-existing class");
}

Class& TypeManager::get_type(const std::string &name)
{
	return get_type(Class::get_id(name));
}

Value TypeManager::invoke_method(Object &obj, const std::string &name, Args &args)
{
	Class &type = *obj.get_class();

	if constexpr (DEBUG)
		std::cout << "Invoking " << obj << "->" << name << std::endl;

	return type.invoke_method(obj, name, args);
}

Object TypeManager::create_object(size_t type_id, Args &args)
{
	return get_type(type_id).create(args);
}

Object TypeManager::create_object(const std::string &type_name, Args &args)
{
	return create_object(std::hash<std::string> { }(type_name), args);
}

Object TypeManager::create_uninitialized_object(Class *type)
{
	return type->create_uninitialized();
}

Class* TypeManager::get_root()
{
	return root.get();
}

} /* namespace mtl */
