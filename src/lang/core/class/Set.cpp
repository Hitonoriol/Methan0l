#include <lang/core/class/Set.h>

#include <iterator>
#include <lang/core/class/List.h>
#include <util/util.h>

namespace mtl
{

NATIVE_CLASS_BINDING(Set, {
	IMPLEMENTS_COLLECTION
	BIND_CONSTRUCTOR(const mtl::native::List&)
	BIND_METHOD(to_string)
})

namespace native
{

Set::Set(const List &list)
	: ContainerWrapper<wrapped_type>(ValSet(list.begin(), list.end())) {}

std::string Set::to_string()
{
	return stringify_container(nullptr, contained);
}

Value Set::remove(Value value)
{
	contained.erase(value);
	return value;
}

UInt Set::size()
{
	return contained.size();
}

Value Set::get(Int idx)
{
	return *std::next(begin(), idx);
}

void Set::clear()
{
	contained.clear();
}

UInt Set::index_of(Value value)
{
	return mtl::index_of(contained, value);
}

Boolean Set::contains(Value value)
{
	return mtl::contains(contained, value);
}

Boolean Set::is_empty()
{
	return contained.empty();
}

Boolean Set::add(Value value)
{
	return contained.insert(value).second;
}

}

} /* namespace mtl */
