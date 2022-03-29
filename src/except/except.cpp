#include "except.h"

#include <string>

#include "../util/util.h"
#include "../structure/Value.h"
#include "../expression/Expression.h"
#include "../type.h"

namespace mtl
{

Exception::Exception(const std::string &msg) : msg(msg)
{
}

const std::string_view& Exception::what()
{
	return msg;
}

Exception& Exception::operator=(const Exception &rhs)
{
	msg = rhs.msg;
	return *this;
}

InvalidTypeException::InvalidTypeException(Type type, Type expected,
		const std::string &msg) :
		std::runtime_error(msg
				+ " received \""
				+ str(Value::type_name(type))
				+ "\" but expected \"" +
				(expected != Type::END ? str(Value::type_name(expected)) : "Unknown")
				+ "\""
				)
{
}

InvalidTypeException::InvalidTypeException(Type type) : InvalidTypeException(type, Type::END)
{
}

}
