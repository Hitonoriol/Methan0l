#ifndef SRC_LANG_CORE_CLASS_SET_H_
#define SRC_LANG_CORE_CLASS_SET_H_

#include <type.h>
#include <lang/core/class/Collection.h>
#include <structure/object/NativeClass.h>
#include <structure/Wrapper.h>
#include <lang/core/class/DefaultIterator.h>

namespace mtl
{

namespace native
{

class List;

class Set : public ContainerWrapper<ValSet>, public Collection
{
	public:
		using iterator_type = SetIterator::bound_class;
		using ContainerWrapper<wrapped_type>::ContainerWrapper;
		Set(const List&);

		Boolean add(Value) override;
		Value remove(Value) override;
		Value remove_at(UInt) override UNIMPLEMENTED
		UInt size() override;
		Value get(Int) override;
		void clear() override;
		UInt index_of(Value) override;
		Boolean contains(Value) override;
		Value append() override UNIMPLEMENTED
		Boolean is_empty() override;

		std::string to_string();
};

}

NATIVE_CLASS(Set, native::Set)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_SET_H_ */
