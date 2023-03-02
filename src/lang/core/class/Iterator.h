#ifndef SRC_LANG_CORE_CLASS_ITERATOR_H_
#define SRC_LANG_CORE_CLASS_ITERATOR_H_

#include <type.h>
#include <except/except.h>
#include <structure/Value.h>
#include <structure/object/OOPDefs.h>
#include <structure/object/NativeClass.h>

namespace mtl
{

class Iterator
{
	public:
		Iterator() = default;
		virtual ~Iterator() = default;

		virtual Value peek() UNIMPLEMENTED

		virtual Value next() UNIMPLEMENTED
		virtual bool has_next() UNIMPLEMENTED

		virtual void reverse() UNIMPLEMENTED

		virtual Value previous() UNIMPLEMENTED
		virtual bool has_previous() UNIMPLEMENTED

		virtual Value remove() UNIMPLEMENTED
};

class IteratorAdapter : public Iterator, public Adapter
{
	public:
		using Adapter::Adapter;

		Value peek() override ADAPTER_METHOD(peek)

		Value next() override ADAPTER_METHOD(next)
		bool has_next() override ADAPTER_METHOD(has_next)

		void reverse() override ADAPTER_VOID_METHOD(reverse)

		Value previous() override ADAPTER_METHOD(previous)
		bool has_previous() override ADAPTER_METHOD(has_previous)

		Value remove() override ADAPTER_METHOD(remove)
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_ITERATOR_H_ */
