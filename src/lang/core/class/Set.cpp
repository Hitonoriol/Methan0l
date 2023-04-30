#include "Set.h"

#include <iterator>

#include <lang/core/class/List.h>
#include <util/util.h>
#include <lang/core/Data.h>

namespace mtl
{

NATIVE_CLASS_BINDING(Set, {
	IMPLEMENTS_COLLECTION
	BIND_CONSTRUCTOR(const mtl::native::List&)
	BIND_METHOD_AS(IndexOperator::Get, contains)
	BIND_METHOD_AS(IndexOperator::Remove, remove)

	BIND_METHOD(intersect)
	BIND_METHOD_AS("union", union_set)
	BIND_METHOD(diff)
	BIND_METHOD(symdiff)

	EQUALITY_COMPARABLE
	BIND_METHOD(to_string)
	BIND_METHOD(hash_code)
})

namespace native
{

Set::Set(const List &list)
	: ContainerWrapper<wrapped_type>(ValSet(list.begin(), list.end())) {}

Value Set::to_string(Context context)
{
	return core::stringify_container(*context, contained);
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

Value Set::add(Value value)
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

Int Set::hash_code()
{
	return mtl::value_container_hash_code(contained);
}

}

} /* namespace mtl */
