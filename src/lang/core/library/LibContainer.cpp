#include <lang/core/library/LibContainer.h>

#include <lang/core/library/container/Pair.h>
#include <lang/core/class/ListIterator.h>

namespace mtl
{

METHAN0L_LIBRARY(LibContainer)

void LibContainer::load()
{
	context->register_class<Pair>();
	context->register_class<ListIterator>();
}

} /* namespace mtl */
