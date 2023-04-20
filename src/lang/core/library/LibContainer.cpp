#include <lang/core/library/LibContainer.h>

#include <CoreLibrary.h>
#include <lang/core/library/container/Pair.h>

namespace mtl
{

METHAN0L_LIBRARY(LibContainer)

void LibContainer::load()
{
	REGISTER_METHAN0L_CLASS(*context, Iterable)
	REGISTER_METHAN0L_CLASS(*context, Iterator)
	REGISTER_METHAN0L_CLASS(*context, Range)
	REGISTER_CLASS_BINDING(*context, Collection)
	REGISTER_CLASS_BINDING(*context, String)
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
	REGISTER_CLASS_BINDING(*context, Mapping)
	REGISTER_CLASS_BINDING(*context, MappingIterator)
	REGISTER_CLASS_BINDING(*context, Filter)
	REGISTER_CLASS_BINDING(*context, FilterIterator)
}

} /* namespace mtl */
