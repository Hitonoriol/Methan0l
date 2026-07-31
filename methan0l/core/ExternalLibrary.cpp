#include "ExternalLibrary.h"

#include <boost/dll.hpp>

#include <methan0l/core/Library.h>
#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/core/Module.h>

namespace mtl
{

void ExternalLibrary::load(Interpreter &context)
{
	auto loader = IMPORT<library_loader>(dll, STR(LIB_LOADER_SYMBOL));
	context.load_library(loader(context));
}

}
