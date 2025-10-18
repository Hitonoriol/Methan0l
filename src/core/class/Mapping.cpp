#include "Mapping.h"

#include <lang/Grammar.h>

namespace mtl
{

NATIVE_CLASS_BINDING(Mapping, {
	IMPLEMENTS(Iterable)
	BIND_NATIVE_ITERATOR()
})

NATIVE_CLASS_BINDING(MappingIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

namespace native
{

MappingIterator::MappingIterator(Mapping &mapping, Interpreter &context)
	: context(context),
	  mapping(mapping),
	  it(mapping.iterable.iterator()),
	  mapping_arg(LiteralExpr::empty()),
	  mapping_arglist(context.allocator<ExprPtr>())
{
	mapping_arglist.push_back(mapping_arg);
}

void MappingIterator::set_arg(const Value &val)
{
	mapping_arg->raw_ref() = val;
}

Value MappingIterator::map()
{
	return context.invoke(mapping.mapper, mapping_arglist);
}

Value MappingIterator::next()
{
	set_arg(it.next());
	return map();
}

Value MappingIterator::previous()
{
	set_arg(it.previous());
	return map();
}

bool MappingIterator::has_previous()
{
	return it.has_previous();
}

bool MappingIterator::can_skip(Int n)
{
	return it.can_skip(n);
}

Value MappingIterator::skip(Int n)
{
	set_arg(it.skip(n));
	return map();
}

bool MappingIterator::has_next()
{
	return it.has_next();
}

void MappingIterator::reverse()
{
	it.reverse();
}

Value MappingIterator::remove()
{
	return it.remove();
}

Value MappingIterator::peek()
{
	set_arg(it.peek());
	return map();
}

Int Mapping::hash_code()
{
	return Value(iterable.get_object()).hash_code() ^ mapper.hash_code();
}

}

} /* namespace mtl */
