#include <lang/core/class/Set.h>

#include <iterator>
#include <lang/core/class/List.h>
#include <util/util.h>

namespace mtl
{

NATIVE_CLASS_BINDING(Set, {
	IMPLEMENTS_COLLECTION
	BIND_CONSTRUCTOR(const mtl::native::List&)
	BIND_METHOD_AS(Operators::Get, contains)
	BIND_METHOD_AS(Operators::Remove, remove)

	BIND_METHOD(intersect)
	BIND_METHOD_AS("union", union_set)
	BIND_METHOD(diff)
	BIND_METHOD(symdiff)

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

Value Set::intersect(Value &b)
{
	auto c = b.get_context().make<Set>();
		return set_operation(*this, b, c, [&](auto &a, auto &b, auto &c) {
			std::copy_if(a.begin(), a.end(), std::inserter(c, c.begin()),
							[&b](auto &element){return b.count(element) > 0;});
		});
}

Value Set::union_set(Value &b)
{
	auto c = b.get_context().make<Set>();
		return set_operation(*this, b, c, [&](auto &a, auto &b, auto &c) {
			c.insert(a.begin(), a.end());
			c.insert(b.begin(), b.end());
		});
}

Value Set::diff(Value &b)
{
	auto c = b.get_context().make<Set>();
	return set_operation(*this, b, c, set_diff);
}

Value Set::symdiff(Value &b)
{
	auto c = b.get_context().make<Set>();
		return set_operation(*this, b, c, [&](auto &a, auto &b, auto &c) {
			set_diff(a, b, c);
			set_diff(b, a, c);
		});
}

}

} /* namespace mtl */
