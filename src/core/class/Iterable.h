#ifndef SRC_LANG_CORE_CLASS_ITERABLE_H_
#define SRC_LANG_CORE_CLASS_ITERABLE_H_

#include <core/class/Iterator.h>
#include <except/except.h>
#include <oop/NativeClass.h>
#include <lang/Grammar.h>

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
			auto &native = *this_obj.get_native().get<std::shared_ptr<T>>();
			auto &context = CONTEXT;
			return context.new_object<Iter>(native, std::ref(context));
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
			...
	};
*/

class IterableAdapter : public Adapter
{
	public:
		using Adapter::Adapter;

		IteratorAdapter iterator() ADAPTER_FACTORY_METHOD(iterator)

		void for_each(Value action) ADAPTER_VOID_METHOD(for_each, action)
		Value map(Value mapper) ADAPTER_METHOD(for_each, mapper)
		Value filter(Value predicate) ADAPTER_METHOD(filter, predicate)
		Value accumulate(Value action) ADAPTER_METHOD(accumulate, action)
		Value collect(Value collection) ADAPTER_METHOD(collect, collection)

		Boolean contains(Value elem) ADAPTER_METHOD(contains, elem)
		Int count() ADAPTER_METHOD(count)

		Float sum() ADAPTER_METHOD(sum)
		Float product() ADAPTER_METHOD(product)
		Float mean() ADAPTER_METHOD(mean)
		Float rms() ADAPTER_METHOD(rms)
		Float deviation() ADAPTER_METHOD(deviation)
};

} /* namespace mtl */

#define BIND_NATIVE_ITERATOR() \
	BIND_EXTERNAL_METHOD(native::IteratorProvider<bound_class>::iterator)

#endif /* SRC_LANG_CORE_CLASS_ITERABLE_H_ */
