#include "Map.h"

namespace mtl
{

NATIVE_CLASS_BINDING(Map, {
	IMPLEMENTS_ABSTRACT_MAP
	BIND_METHOD(to_string)
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

std::string Map::to_string()
{
	auto it = contained.begin(), end = contained.end();
	return stringify([&] {
		if (it == end) return empty_string;
		auto str = "{" + unconst(it->first).to_string() + ": " + it->second.to_string() + "}";
		++it;
		return str;
	});
}

}

} /* namespace mtl */
