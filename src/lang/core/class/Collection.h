#ifndef SRC_LANG_CORE_CLASS_COLLECTION_H_
#define SRC_LANG_CORE_CLASS_COLLECTION_H_

#include <type.h>
#include <except/except.h>
#include <structure/Value.h>
#include <lang/core/class/Iterable.h>
#include <Grammar.h>

namespace mtl
{

namespace native
{

class Collection
{
	public:
		virtual ~Collection() = default;

		virtual Boolean add(Value) UNIMPLEMENTED
		virtual Value append() UNIMPLEMENTED
		virtual Value remove(Value) UNIMPLEMENTED
		virtual Value remove_at(UInt) UNIMPLEMENTED
		virtual Value get(Int) UNIMPLEMENTED
		virtual UInt size() UNIMPLEMENTED
		virtual void resize(UInt) UNIMPLEMENTED
		virtual void clear() UNIMPLEMENTED
		virtual Boolean is_empty() UNIMPLEMENTED
		virtual UInt index_of(Value) UNIMPLEMENTED
		virtual Boolean contains(Value) UNIMPLEMENTED
};

} /* namespace native */

NATIVE_CLASS(Collection, native::Collection)

class CollectionAdapter : public native::Collection, public IterableAdapter
{
	public:
		using IterableAdapter::IterableAdapter;
		Boolean add(Value elem) override ADAPTER_METHOD(add, elem)
		Value append() override ADAPTER_METHOD(append)
		Value remove(Value elem) override ADAPTER_METHOD(remove, elem)
		Value remove_at(UInt idx) override ADAPTER_METHOD(remove_at, idx)
		Value get(Int idx) override ADAPTER_METHOD(get, idx)
		UInt size() override ADAPTER_METHOD(size)
		void resize(UInt nsize) override ADAPTER_VOID_METHOD(resize, nsize)
		void clear() override ADAPTER_VOID_METHOD(clear)
		Boolean is_empty() override ADAPTER_METHOD(is_empty)
		UInt index_of(Value elem) override ADAPTER_METHOD(index_of, elem)
		Boolean contains(Value elem) override ADAPTER_METHOD(contains, elem)
};

#define COLLECTION_BINDINGS \
	BIND_METHOD(add) \
	BIND_METHOD(append) \
	BIND_METHOD(remove) \
	BIND_METHOD(remove_at) \
	BIND_METHOD(get) \
	BIND_METHOD(size) \
	BIND_MUTATOR_METHOD(resize) \
	BIND_MUTATOR_METHOD(clear) \
	BIND_METHOD(is_empty) \
	BIND_METHOD(index_of) \
	BIND_METHOD(contains) \
	/* Subscript operator overloads: */ \
	BIND_METHOD_AS(IndexOperator::Get, get) \
	BIND_METHOD_AS(IndexOperator::Append, append) \
	BIND_METHOD_AS(IndexOperator::Remove, remove_at) \
	BIND_METHOD_AS(IndexOperator::Clear, clear) \
	BIND_METHOD_AS(IndexOperator::Insert, add) \
	METHOD_ALIAS(for_each, IndexOperator::Foreach) \
	METHOD_ALIAS(slice, IndexOperator::Slice)

#define IMPLEMENTS_COLLECTION \
	IMPLEMENTS(Collection) \
	BIND_CONSTRUCTOR() \
	BIND_NATIVE_ITERATOR() \
	COLLECTION_BINDINGS

}

#endif /* SRC_LANG_CORE_CLASS_COLLECTION_H_ */
