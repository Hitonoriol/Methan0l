#ifndef SRC_LANG_CORE_CLASS_SET_H_
#define SRC_LANG_CORE_CLASS_SET_H_

#include <type.h>
#include <lang/core/class/Collection.h>
#include <oop/NativeClass.h>
#include <structure/Wrapper.h>
#include <lang/core/class/DefaultIterator.h>

#include <algorithm>

namespace mtl
{

namespace native
{

class List;

class Set : public ContainerWrapper<ValSet>, public Collection
{
	private:
		/* Set operation where `args` contains 2 set expressions */
		template<typename T>
		inline Value& set_operation(Set &a, Set &b, Value &result, T &&operation)
		{
			operation(*a, *b, *result.get<Set>());
			return result;
		}

		static void set_diff(ValSet &a, ValSet &b, ValSet &c)
		{
			std::copy_if(a.begin(), a.end(), std::inserter(c, c.begin()),
					[&b](auto &element) {return b.count(element) == 0;});
		}

	public:
		using iterator_type = SetIterator::bound_class;
		using ContainerWrapper<wrapped_type>::ContainerWrapper;
		Set(const List&);

		Value add(Value) override;
		Value remove(Value) override;
		Value remove_at(UInt) override UNIMPLEMENTED
		UInt size() override;
		Value get(Int) override;
		void clear() override;
		UInt index_of(Value) override;
		Boolean contains(Value) override;
		Value append() override UNIMPLEMENTED
		Boolean is_empty() override;

		Value intersect(Value &b);
		Value union_set(Value &b);
		Value diff(Value &b);
		Value symdiff(Value &b);

		WRAPPER_EQUALS_COMPARABLE(Set)
		Value to_string(Context);
		Int hash_code();
};

}

NATIVE_CLASS(Set, native::Set)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_SET_H_ */
