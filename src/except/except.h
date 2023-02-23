#ifndef SRC_EXCEPT_EXCEPT_H_
#define SRC_EXCEPT_EXCEPT_H_

#include <stdexcept>

#include "type.h"
#include "lang/DataType.h"

#define UNIMPLEMENTED { throw mtl::IllegalOperationException(); }

namespace mtl
{

class Exception {
	private:
		std::string_view msg;

	public:
		Exception(const std::string &);
		Exception& operator=(const Exception &rhs);
		const std::string_view &what();
};

struct InvalidTypeException : public std::runtime_error
{
		InvalidTypeException(Value received, TypeID expected);
		InvalidTypeException(TypeID type, TypeID expected, const std::string &msg = "Invalid type conversion:");
		InvalidTypeException(TypeID type);
};

class IllegalOperationException : public std::runtime_error
{
	public:
		IllegalOperationException() : std::runtime_error("Illegal operation") {}
};

} /* namespace mtl */

#endif /* SRC_EXCEPT_EXCEPT_H_ */
