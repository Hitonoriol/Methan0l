#include "except.h"

#include <string>

#include "util/util.h"
#include "structure/Value.h"
#include "expression/Expression.h"
#include "util/global.h"
#include "interpreter/Interpreter.h"
#include "CoreLibrary.h"

namespace mtl
{

Exception::Exception(const std::string &msg) : msg(msg)
{}

const std::string_view& Exception::what()
{
	return msg;
}

Exception& Exception::operator=(const Exception &rhs)
{
	msg = rhs.msg;
	return *this;
}

std::string type_error_msg(const std::string &msg, TypeID type, TypeID expected)
{
	return msg + " expected \""
			+ (expected != TypeID::NONE ? str(expected.type_name()) : "<unspecified>")
			+ "\""
			+ " but received \"" + str(type.type_name()) + "\"";
}

InvalidTypeException::InvalidTypeException(TypeID type, TypeID expected, const std::string &msg) :
		std::runtime_error(type_error_msg(msg, type, expected))
{}

InvalidTypeException::InvalidTypeException(Value received, TypeID expected) :
		std::runtime_error(type_error_msg("Invalid conversion of value `"
				+ mtl::str(received.to_string()) + "`:",
				received.type(), expected))
{}

InvalidTypeException::InvalidTypeException(TypeID type) : InvalidTypeException(type, TypeID::NONE)
{}

}
