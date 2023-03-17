#ifndef SRC_LANG_CORE_CLASS_ITERABLE_H_
#define SRC_LANG_CORE_CLASS_ITERABLE_H_

#include <except/except.h>
#include <oop/NativeClass.h>
#include <Grammar.h>
#include <lang/core/class/Iterator.h>


namespace mtl
{

namespace native
{

template<class T>
class IteratorProvider
{
	public:
		template<typename Iter = typename T::iterator_type>
		static Object iterator(OBJ)
		{
			return CONTEXT.new_object<Iter>(this_obj.get_native().get<T>());
		}
};

} /* namespace native */

METHAN0L_CLASS(Iterable)
/*     Implied ^^^ , but exists only as a methan0l class: */
/*
	class Iterable
	{
		public:
			virtual Value iterator();

			virtual void for_each(Value) UNIMPLEMENTED
			virtual Value map(Value) UNIMPLEMENTED
			virtual Int accumulate(Value) UNIMPLEMENTED
			virtual Int sum() UNIMPLEMENTED
	};
*/

class IterableAdapter : public Adapter
{
	public:
		using Adapter::Adapter;

		IteratorAdapter iterator() ADAPTER_METHOD(iterator)
};

} /* namespace mtl */

#define IMPLEMENTS_ITERABLE ABSTRACT_METHOD(iterator)

#define BIND_NATIVE_ITERATOR() \
	BIND_EXTERNAL_METHOD(native::IteratorProvider<bound_class>::iterator)

#endif /* SRC_LANG_CORE_CLASS_ITERABLE_H_ */
