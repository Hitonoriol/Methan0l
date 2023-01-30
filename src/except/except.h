#ifndef SRC_EXCEPT_EXCEPT_H_
#define SRC_EXCEPT_EXCEPT_H_

#include <stdexcept>

#include "../type.h"

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

enum class Type : uint8_t;
struct InvalidTypeException : public std::runtime_error
{
		InvalidTypeException(Value received, Type expected);
		InvalidTypeException(Type type, Type expected, const std::string &msg = "Invalid type conversion:");
		InvalidTypeException(Type type);
};

class IllegalOperationException : public std::runtime_error
{
	public:
		IllegalOperationException() : std::runtime_error("Illegal operation") {}
};

} /* namespace mtl */

#endif /* SRC_EXCEPT_EXCEPT_H_ */
