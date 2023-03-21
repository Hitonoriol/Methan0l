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
			auto &container = *this_obj.get_native().get<std::shared_ptr<T>>();
			return CONTEXT.new_object<Iter>(container);
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
	};
*/

class IterableAdapter : public Adapter
{
	public:
		using Adapter::Adapter;

		IteratorAdapter iterator() ADAPTER_FACTORY_METHOD(iterator)

		void for_each(Value action) ADAPTER_VOID_METHOD(for_each, action)
		Value map(Value mapper) ADAPTER_METHOD(for_each, mapper)
		Value accumulate(Value action) ADAPTER_METHOD(accumulate, action)
};

} /* namespace mtl */

#define BIND_NATIVE_ITERATOR() \
	BIND_EXTERNAL_METHOD(native::IteratorProvider<bound_class>::iterator)

#endif /* SRC_LANG_CORE_CLASS_ITERABLE_H_ */
