#include "ExternalLibrary.h"

#include <boost/dll.hpp>

#include <core/Library.h>
#include <interpreter/Interpreter.h>
#include <core/Module.h>

namespace mtl
{

void ExternalLibrary::load(Interpreter &context)
{
	auto loader = IMPORT<library_loader>(dll, STR(LIB_LOADER_SYMBOL));
	context.load_library(loader(context));
}

}
