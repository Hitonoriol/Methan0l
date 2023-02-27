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

List::List() {}
List::List(const ValList &other) : list(other) {}
List::List(const List &rhs) : list(rhs.list) {}

Object List::iterator(OBJ)
{
	return CONTEXT.new_object<ListIterator>(NATIVE(mtl::List).list);
}

void List::add(Value val)
{
	list.push_back(val);
}

Value List::remove_at(UInt idx)
{
	auto removed = list.at(idx);
	list.erase(list.begin() + idx);
	return removed;
}

Value List::remove(Value val)
{
	for (auto it = list.begin(); it != list.end(); ++it) {
		if (*it == val) {
			auto removed = *it;
			list.erase(it);
			return removed;
		}
	}
	return Value::NIL;
}

std::string List::to_string()
{
	return stringify_container(nullptr, list);
}

}

} /* namespace mtl */
