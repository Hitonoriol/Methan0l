#include <lang/SharedLibrary.h>

namespace mtl
{

SharedLibrary::SharedLibrary(const std::string &path)
	: dll(path)
{
	if (!dll.is_loaded())
		throw std::runtime_error("couldn't access library file");
}

SharedLibrary::SharedLibrary(const SharedLibrary &rhs)
	: dll(rhs.dll) {}

} /* namespace mtl */
