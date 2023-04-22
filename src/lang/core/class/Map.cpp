#include "Map.h"

#include <CoreLibrary.h>

namespace mtl
{

NATIVE_CLASS_BINDING(Map, {
	IMPLEMENTS_ABSTRACT_MAP
	BIND_METHOD_AS(IndexOperator::Get, operator_get)

	EQUALITY_COMPARABLE
	BIND_METHOD(to_string)
	BIND_METHOD(hash_code)
})

namespace native
{

Map::Map() {}

Boolean Map::contains_key(Value key)
{
	return contained.find(key) != contained.end();
}

Value Map::add(Value key, Value value)
{
	auto it = contained.find(key);
	auto previous = it == contained.end() ? Value::NIL : it->second;
	contained[key] = value;
	return previous;
}

Value Map::remove(Value key)
{
	return contained.erase(key) != 0;
}

Value Map::get(Value key)
{
	auto it = contained.find(key);
	if (it == contained.end())
		return Value::NIL;
	return it->second;
}

Value Map::operator_get(Value key)
{
	return Value::ref(contained[key]);
}

UInt Map::size()
{
	return contained.size();
}

void Map::clear()
{
	contained.clear();
}

Boolean Map::is_empty()
{
	return contained.empty();
}

Value Map::to_string(Context context)
{
	auto it = contained.begin(), end = contained.end();
	return context->make<String>(std::move(stringify([&] {
		if (it == end) return empty_string;
		auto str = "{" + unconst(it->first).to_string() + ": " + it->second.to_string() + "}";
		++it;
		return str;
	})));
}

Int Map::hash_code()
{
	size_t hash = size();
	for (auto &entry : contained)
		hash ^= entry.first.hash_code() + entry.second.hash_code();
	return hash;
}

}

} /* namespace mtl */
