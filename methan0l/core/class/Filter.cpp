#include "Filter.h"

namespace mtl
{

NATIVE_CLASS_BINDING(Filter, {
	IMPLEMENTS(Iterable)
	BIND_NATIVE_ITERATOR()
})

NATIVE_CLASS_BINDING(FilterIterator, {
	IMPLEMENTS(Iterator)
	ITERATOR_BINDINGS
})

namespace native
{

FilterIterator::FilterIterator(Filter &filter, Interpreter &context)
	: context(context), filter(filter),
	  it(filter.iterable.iterator()),
	  predicate_arg(LiteralExpr::empty()),
	  predicate_arglist(context.allocator<ExprPtr>())
{
	predicate_arglist.push_back(predicate_arg);
}

Value FilterIterator::next()
{
	next_value();
	return consume();
}

bool FilterIterator::has_next()
{
	next_value();
	return !value.empty();
}

Value FilterIterator::remove()
{
	return it.remove();
}

Value FilterIterator::peek()
{
	return value;
}

bool FilterIterator::can_skip(Int n)
{
	return it.can_skip(n);
}

Value FilterIterator::skip(Int n)
{
	return it.skip(n);
}

}

} /* namespace mtl */
