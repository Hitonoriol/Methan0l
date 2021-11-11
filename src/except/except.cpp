#include "except.h"

#include <string>

#include "../util/util.h"
#include "../structure/Value.h"
#include "../expression/Expression.h"
#include "../type.h"

namespace mtl
{

InvalidTypeException::InvalidTypeException(Type type, Type expected, const std::string &msg) :
		std::runtime_error(msg + " "
				+ str(Value::type_name(type)) + " -> " +
				(expected != Type::END ? str(Value::type_name(expected)) : "Unknown"))
{
}

InvalidTypeException::InvalidTypeException(Type type) : InvalidTypeException(type, Type::END)
{
}

}
