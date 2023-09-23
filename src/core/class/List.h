#ifndef SRC_LANG_CORE_CLASS_LIST_H_
#define SRC_LANG_CORE_CLASS_LIST_H_

#include "Collection.h"
#include "DefaultIterator.h"
#include "GenericContainer.h"

#include <oop/NativeClass.h>
#include <structure/Wrapper.h>

namespace mtl
{

NATIVE_CLASS(ListIterator, native::DefaultIterator<ValList>)

namespace native
{

class List : public GenericContainer<ValList>
{
	public:
		using iterator_type = ListIterator::bound_class;
		using GenericContainer<wrapped_type>::GenericContainer;
		WRAPPER_EQUALS_COMPARABLE(List)

		Value to_string(Context context);
		Int hash_code();
};

}

NATIVE_CLASS(List, native::List)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_LIST_H_ */
