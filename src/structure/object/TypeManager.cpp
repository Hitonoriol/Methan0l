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

TypeManager::TypeManager(Interpreter &eval)
		: eval(eval), root(new Anonymous(eval))
{}

TypeManager::~TypeManager()
{
	delete root;
}

void TypeManager::register_type(std::shared_ptr<Class> type)
{
	if constexpr (DEBUG)
		out << "! Registering type ["
				<< std::hex << type->get_id() << "] " << std::dec
				<< type->get_class_data() << std::endl;

	types.emplace(type->get_id(), type);
}

void TypeManager::unregister_type(const std::string &name)
{
	types.erase(Class::get_id(name));
}

Class& TypeManager::get_type(size_t id)
{
	auto entry = types.find(id);
	if (entry != types.end())
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

Class* TypeManager::get_root()
{
	return root;
}

} /* namespace mtl */
