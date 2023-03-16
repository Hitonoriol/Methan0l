#include "List.h"
#include "DefaultIterator.h"

namespace mtl
{

NATIVE_CLASS_BINDING(List, {
	IMPLEMENTS_COLLECTION
	BIND_METHOD(to_string)
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

std::string List::to_string()
{
	return stringify_container(nullptr, contained);
}

UInt List::size()
{
	return contained.size();
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

}

} /* namespace mtl */
