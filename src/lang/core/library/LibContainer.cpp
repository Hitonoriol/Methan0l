#include <lang/core/library/LibContainer.h>

#include <lang/core/library/container/Pair.h>
#include <lang/core/class/List.h>
#include <lang/core/class/Set.h>
#include <lang/core/class/Map.h>
#include <lang/core/class/Range.h>

namespace mtl
{

METHAN0L_LIBRARY(LibContainer)

void LibContainer::load()
{
	REGISTER_METHAN0L_CLASS(*context, Iterable)
	REGISTER_METHAN0L_CLASS(*context, Iterator)
	REGISTER_CLASS_BINDING(*context, Collection)
	REGISTER_CLASS_BINDING(*context, AbstractMap)
	REGISTER_CLASS_BINDING(*context, Pair)
	REGISTER_CLASS_BINDING(*context, List)
	REGISTER_CLASS_BINDING(*context, ListIterator)
	REGISTER_CLASS_BINDING(*context, Set)
	REGISTER_CLASS_BINDING(*context, SetIterator)
	REGISTER_CLASS_BINDING(*context, Map)
	REGISTER_CLASS_BINDING(*context, MapEntry)
	REGISTER_CLASS_BINDING(*context, MapIterator)
	REGISTER_CLASS_BINDING(*context, IntRange)
	REGISTER_CLASS_BINDING(*context, IntRangeIterator)
	REGISTER_CLASS_BINDING(*context, FloatRange)
	REGISTER_CLASS_BINDING(*context, FloatRangeIterator)
}

} /* namespace mtl */
