#include "Buffer.h"

#include <core/Data.h>

namespace mtl
{

NATIVE_CLASS_BINDING(Buffer, {
	IMPLEMENTS_COLLECTION
	EQUALITY_COMPARABLE
	BIND_MUTATOR_METHOD(add)

	BIND_DARGS_METHOD(get_integer, -1)
	BIND_DARGS_METHOD(get_float, -1)
	BIND_DARGS_METHOD(add_integer, -1)
	BIND_DARGS_METHOD(add_float, -1)
	BIND_DARGS_METHOD(insert_integer, -1)
	BIND_DARGS_METHOD(insert_float, -1)

	BIND_METHOD(to_string)
	BIND_METHOD(hash_code)
})

NATIVE_CLASS_BINDING(BufferIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

bool operator== (const uint8_t& rhs, const Value& lhs)
{
	return rhs == lhs.as<uint8_t>();
}

namespace native
{

Int Buffer::get_integer(Int idx, Int size)
{
	return get_as<Int>(idx, size);
}

Float Buffer::get_float(Int idx, Int size)
{
	return get_as<Float>(idx, size);
}

Value Buffer::add(Value val)
{
	contained.push_back(val);
	return contained.size() - 1;
}

Int Buffer::add_integer(Int val, Int size)
{
	auto pos = contained.size();
	add_as<Int>(val, size);
	return pos;
}

Int Buffer::add_float(Float val, Int size)
{
	auto pos = contained.size();
	add_as<Float>(val, size);
	return pos;
}

void Buffer::insert_integer(Int idx, Int val, Int size)
{
	add_as<Int>(val, size, idx);
}

void Buffer::insert_float(Int idx, Float val, Int size)
{
	add_as<Float>(val, size, idx);
}

Value Buffer::to_string(Context context)
{
	auto it = std::begin(contained), end = std::end(contained);
	return context->make<String>(
		mtl::stringify([&] {
			if (it == end)
				return empty_string;
			return mtl::to_base(*(it++), 16);
		})
	);
}

Int Buffer::hash_code()
{
	Int hash = contained.size();
	for (auto &byte : contained)
		hash ^= byte + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

}

} /* namespace mtl */
