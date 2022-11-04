#include "except.h"

#include <string>

#include "../util/util.h"
#include "../structure/Value.h"
#include "../expression/Expression.h"
#include "../type.h"
#include "util/global.h"
#include "interpreter/ExprEvaluator.h"

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

std::string error_msg(const std::string &msg, Type type, Type expected)
{
	return msg + " expected \""
			+ (expected != Type::END ? str(Value::type_name(expected)) : "<unspecified>")
			+ "\""
			+ " but received \"" + str(Value::type_name(type)) + "\"";
}

InvalidTypeException::InvalidTypeException(Type type, Type expected, const std::string &msg) :
		std::runtime_error(error_msg(msg, type, expected))
{}

InvalidTypeException::InvalidTypeException(Value received, Type expected) :
		std::runtime_error(error_msg("Invalid conversion of value `"
				+ received.to_string() + "`:",
				received.type(), expected))
{}

InvalidTypeException::InvalidTypeException(Type type) : InvalidTypeException(type, Type::END)
{}

}
