#include "DataType.h"

#include <oop/Object.h>
#include <structure/ValueRef.h>
#include <CoreLibrary.h>

namespace mtl
{

#define DATA_TYPE_DEF(name, bound_name, type) \
	const TypeID Type::name{typeid(type), STR(bound_name)};

#define DATA_TYPE(name, type) DATA_TYPE_DEF(name, type, type)
#define HEAP_DATA_TYPE(name, type) DATA_TYPE_DEF(name, type, V##type)

DATA_TYPE(NIL, Nil)
DATA_TYPE(INTEGER, Int)
DATA_TYPE(DOUBLE, Float)
DATA_TYPE(BOOLEAN, Boolean)
HEAP_DATA_TYPE(UNIT, Unit)
HEAP_DATA_TYPE(FUNCTION, Function)
DATA_TYPE(CHAR, Character)
DATA_TYPE(OBJECT, Object)
DATA_TYPE(REFERENCE, ValueRef)
HEAP_DATA_TYPE(TOKEN, Token)					// To be removed
DATA_TYPE_DEF(EXPRESSION, Expression, ExprPtr)
DATA_TYPE(FALLBACK, std::any)

/* Non-built-in types */
DATA_TYPE(STRING, std::shared_ptr<native::String>)

#define TYPE_INDEX(type) {type.type_id(), &type}
Type Type::instance;
Type::Type()
	: type_index ({
		TYPE_INDEX(NIL),
		TYPE_INDEX(INTEGER),
		TYPE_INDEX(DOUBLE),
		TYPE_INDEX(BOOLEAN),
		TYPE_INDEX(UNIT),
		TYPE_INDEX(FUNCTION),
		TYPE_INDEX(CHAR),
		TYPE_INDEX(OBJECT),
		TYPE_INDEX(REFERENCE),
		TYPE_INDEX(TOKEN),
		TYPE_INDEX(EXPRESSION),
		TYPE_INDEX(FALLBACK)
	})
{
	for (auto &[id, type] : type_index)
		type_name_index.emplace(type->type_name(), type);
}

const TypeID& Type::get(Int id)
{
	auto type = instance.type_index.find(id);
	if (type != instance.type_index.end())
		return *type->second;

	return TypeID::NONE;
}

const TypeID& Type::get(std::string_view name)
{
	auto type = instance.type_name_index.find(name);
	if (type != instance.type_name_index.end())
		return *type->second;

	return TypeID::NONE;
}

} /* namespace mtl */
