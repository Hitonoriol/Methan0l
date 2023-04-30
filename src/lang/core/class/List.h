#ifndef SRC_LANG_CORE_CLASS_LIST_H_
#define SRC_LANG_CORE_CLASS_LIST_H_

#include "Collection.h"
#include "DefaultIterator.h"

#include <oop/NativeClass.h>
#include <structure/Wrapper.h>

namespace mtl
{

class ListIterator;

namespace native
{

class List : public ContainerWrapper<ValList>, public Collection
{
	public:
		using iterator_type = ListIterator::bound_class;
		using ContainerWrapper<wrapped_type>::ContainerWrapper;

		inline Value& operator[](UInt idx)
		{
			return (contained)[idx];
		}

		inline void push_back(const Value &value)
		{
			contained.push_back(value);
		}

		Value add(Value) override;
		Value remove_at(UInt) override;
		Value remove(Value) override;
		Value get(Int) override;
		UInt size() override;
		void resize(UInt) override;
		void clear() override;
		UInt index_of(Value) override;
		Boolean contains(Value) override;
		Value append() override;
		Boolean is_empty() override;

		WRAPPER_EQUALS_COMPARABLE(List)
		Value to_string(Context);
		Int hash_code();
};

}

NATIVE_CLASS(List, native::List)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_LIST_H_ */
