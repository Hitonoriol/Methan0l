#ifndef SRC_LANG_CORE_CLASS_ABSTRACTMAP_H_
#define SRC_LANG_CORE_CLASS_ABSTRACTMAP_H_

#include <except/except.h>
#include <structure/Value.h>
#include <lang/core/class/Iterable.h>

namespace mtl
{

namespace native
{

class AbstractMap
{
	public:
		virtual ~AbstractMap() = default;

		virtual Value add(Value, Value) UNIMPLEMENTED
		virtual Value remove(Value) UNIMPLEMENTED
		virtual Value get(Value) UNIMPLEMENTED
		virtual Boolean contains_key(Value) UNIMPLEMENTED
		virtual UInt size() UNIMPLEMENTED
		virtual void clear() UNIMPLEMENTED
		virtual Boolean is_empty() UNIMPLEMENTED
};

} /* namespace native */

NATIVE_CLASS(AbstractMap, native::AbstractMap)

class AbstractMapAdapter : public native::AbstractMap, public IterableAdapter
{
	public:
		using IterableAdapter::Adapter;

		virtual Value add(Value key, Value value) override ADAPTER_METHOD(add, key, value)
		virtual Value remove(Value key) override ADAPTER_METHOD(remove, key)
		virtual Value get(Value key) override ADAPTER_METHOD(get, key)
		virtual Boolean contains_key(Value key) override ADAPTER_METHOD(contains_key, key)
		virtual UInt size() override ADAPTER_METHOD(size)
		virtual void clear() override ADAPTER_VOID_METHOD(clear)
		virtual Boolean is_empty() override ADAPTER_METHOD(is_empty)
};

#define ABSTRACT_MAP_BINDINGS \
	BIND_METHOD(add) \
	BIND_METHOD(remove) \
	BIND_METHOD(get) \
	BIND_METHOD(contains_key) \
	BIND_METHOD(size) \
	BIND_METHOD(clear) \
	BIND_METHOD(is_empty) \
	/* Subscript operator overloads: */ \
	BIND_METHOD_AS(IndexOperator::Get, get) \
	BIND_METHOD_AS(IndexOperator::Remove, remove) \
	BIND_METHOD_AS(IndexOperator::Clear, clear) \
	METHOD_ALIAS(for_each, IndexOperator::Foreach)

#define IMPLEMENTS_ABSTRACT_MAP \
	IMPLEMENTS(AbstractMap) \
	BIND_CONSTRUCTOR() \
	BIND_NATIVE_ITERATOR() \
	ABSTRACT_MAP_BINDINGS

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_ABSTRACTMAP_H_ */
