#include <lang/core/library/LibContainer.h>

#include <lang/core/library/container/Pair.h>

namespace mtl
{

METHAN0L_LIBRARY(LibContainer)

void LibContainer::load()
{
	context->register_class<Pair>();
}

} /* namespace mtl */
