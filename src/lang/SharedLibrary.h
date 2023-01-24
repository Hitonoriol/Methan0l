#ifndef SRC_LANG_SHAREDLIBRARY_H_
#define SRC_LANG_SHAREDLIBRARY_H_

#include <boost/dll.hpp>

namespace mtl
{

class Interpreter;

class SharedLibrary
{
	protected:
		boost::dll::shared_library dll;

	public:
		SharedLibrary(const std::string &path);
		SharedLibrary(const SharedLibrary&);
		virtual ~SharedLibrary() = default;

		virtual void load(Interpreter &context) = 0;
};

} /* namespace mtl */

#endif /* SRC_LANG_SHAREDLIBRARY_H_ */
