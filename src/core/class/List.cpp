#include "List.h"

#include <lang/util/string.h>
#include <core/Data.h>

namespace mtl
{

NATIVE_CLASS_BINDING(List, {
	IMPLEMENTS_COLLECTION
	EQUALITY_COMPARABLE
	BIND_MUTATOR_METHOD(add)
	BIND_METHOD(to_string)
	BIND_METHOD(hash_code)
})

NATIVE_CLASS_BINDING(ListIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

namespace native
{

Value List::to_string(Context context)
{
	return context->make<String>(mtl::stringify_container(this->contained));
}

Int List::hash_code()
{
	return mtl::value_container_hash_code(this->contained);
}

}

} /* namespace mtl */
