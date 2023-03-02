#include "List.h"
#include "ListIterator.h"

namespace mtl
{

NATIVE_CLASS_BINDING(List, {
	BIND_CONSTRUCTOR()
	BIND_METHOD(add)
	BIND_METHOD(remove)
	BIND_METHOD(remove_at)
	BIND_METHOD(to_string)
	BIND_PROXY_METHOD(native::List::iterator)
})

namespace native
{

Object List::iterator(OBJ)
{
	return CONTEXT.new_object<ListIterator>(NATIVE(mtl::List).contained);
}

void List::add(Value val)
{
	contained.push_back(val);
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

std::string List::to_string()
{
	return stringify_container(nullptr, contained);
}

}

} /* namespace mtl */
