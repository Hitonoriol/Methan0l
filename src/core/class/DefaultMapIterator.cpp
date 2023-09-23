#include "DefaultMapIterator.h"

#include <CoreLibrary.h>

namespace mtl
{

NATIVE_CLASS_BINDING(MapEntry, {
	BIND_CONSTRUCTOR(Value&, Value&)
	BIND_METHOD(key)
	BIND_METHOD(value)

	STANDARD_METHOD(Methods::ToString) (OBJ) {
		auto &p = THIS;
		return CONTEXT.make<String>(
			"{" + mtl::str(p.key().to_string()) + ": "
			+ mtl::str(p.value().to_string()) + "}"
		);
	};
})

NATIVE_CLASS_BINDING(MapIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

namespace native
{

MapEntry::MapEntry(Value &key, Value &value)
{
	set(key, value);
}

void MapEntry::set(Value &key, Value &value)
{
	contained.first = key;
	contained.second = value;
}

Value MapEntry::key()
{
	return contained.first;
}

Value MapEntry::value()
{
	return contained.second;
}

}

} /* namespace mtl */
