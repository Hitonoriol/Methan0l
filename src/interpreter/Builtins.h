#ifndef SRC_INTERPRETER_BUILTINS_H_
#define SRC_INTERPRETER_BUILTINS_H_

#include <core/Library.h>

namespace mtl
{

/*
 * Library of essential classes required for interpreter to function.
 * Intended to be loaded before any other libraries on interpreter initialization.
 * Despite its name, the classes registered by this library are not built-in types.
 */
class Builtins : public Library
{
	public:
		using Library::Library;
		void load() override;
};

} /* namespace mtl */

#endif /* SRC_INTERPRETER_BUILTINS_H_ */
