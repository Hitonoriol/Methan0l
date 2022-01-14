#include "TypeManager.h"

#include <iomanip>
#include <stdexcept>
#include <utility>

#include "../../expression/LiteralExpr.h"
#include "../../ExprEvaluator.h"
#include "../DataTable.h"
#include "../Function.h"
#include "../Value.h"

namespace mtl
{

void TypeManager::register_type(std::unique_ptr<ObjectType> &&type)
{
	if constexpr (DEBUG)
		out << "! Registering type ["
				<< std::hex << type->get_id() << "] " << std::dec
				<< type->get_class_data() << std::endl;

	types.emplace(type->get_id(), std::move(type));
}

ObjectType& TypeManager::get_type(size_t id)
{
	auto entry = types.find(id);
	if (entry != types.end())
		return *(entry->second);

	throw std::runtime_error("Trying to perform operation on a non-existing Object type");
}

Value TypeManager::invoke_method(Object &obj, const std::string &name, Args &args)
{
	ObjectType &type = get_type(obj.type_id());

	if constexpr (DEBUG)
		std::cout << "Invoking " << obj << "->" << name << std::endl;

	/* First arg of method call is "this" idfr */
	Args arg_copy = args;
	obj.inject_this(arg_copy);
	return type.invoke_method(obj, name, arg_copy);
}

Object TypeManager::create_object(size_t type_id, Args &args)
{
	return get_type(type_id).create(args);
}

Object TypeManager::create_object(const std::string &type_name, Args &args)
{
	return create_object(std::hash<std::string> { }(type_name), args);
}

} /* namespace mtl */
