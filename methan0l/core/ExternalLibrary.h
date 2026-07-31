#ifndef SRC_LANG_EXTERNALLIBRARY_H_
#define SRC_LANG_EXTERNALLIBRARY_H_

#include "SharedLibrary.h"

namespace mtl
{

class ExternalLibrary: public SharedLibrary
{
	public:
		using SharedLibrary::SharedLibrary;
		void load(Interpreter &context) override;
};

}

#endif /* SRC_LANG_EXTERNALLIBRARY_H_ */
