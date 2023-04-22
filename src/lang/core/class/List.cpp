#include "List.h"

#include <lang/core/Data.h>

namespace mtl
{

NATIVE_CLASS_BINDING(List, {
	IMPLEMENTS_COLLECTION
	EQUALITY_COMPARABLE
	BIND_METHOD(to_string)
	BIND_METHOD(hash_code)
})

namespace native
{

Boolean List::add(Value val)
{
	contained.push_back(val);
	return true;
}

Value List::remove_at(UInt idx)
{
	auto removed = contained.at(idx);
	contained.erase(contained.begin() + idx);
	return removed;
}

Value List::remove(Value val)
{
	for (auto it = contained.begin(); it != contained.end(); ++it) {
		if (*it == val) {
			auto removed = *it;
			contained.erase(it);
			return removed;
		}
	}
	return Value::NIL;
}

Value List::get(Int idx)
{
	return contained.at(idx);
}

Value List::to_string(Context context)
{
	return core::stringify_container(*context, contained);
}

UInt List::size()
{
	return contained.size();
}

void List::resize(UInt new_size)
{
	contained.resize(new_size);
}

void List::clear()
{
	contained.clear();
}

UInt List::index_of(Value value)
{
	return mtl::index_of(contained, value);
}

Boolean List::contains(Value value)
{
	return mtl::contains(contained, value);
}

Value List::append()
{
	auto &elem = contained.emplace_back();
	return Value::ref(elem);
}

Boolean List::is_empty()
{
	return contained.empty();
}

Int List::hash_code()
{
	return mtl::value_container_hash_code(contained);
}

}

} /* namespace mtl */
