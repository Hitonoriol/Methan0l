#include "ExternalLibrary.h"

#include <boost/dll.hpp>

#include <lang/Library.h>
#include <interpreter/Interpreter.h>

namespace mtl
{

void ExternalLibrary::load(Interpreter &context)
{
	auto loader = boost::dll::import_symbol<library_loader>(dll, STR(LIB_LOADER_SYMBOL));
	context.load_library(loader(context));
}

}
