#ifndef SRC_LANG_CORE_CLASS_FILTER_H_
#define SRC_LANG_CORE_CLASS_FILTER_H_

#include "Iterable.h"
#include "Iterator.h"

namespace mtl
{

namespace native
{

class FilterIterator;

class Filter
{
	private:
		friend class FilterIterator;

		IterableAdapter iterable;
		Value predicate;

	public:
		using iterator_type = FilterIterator;

		Filter(const Object &obj, const Value &predicate)
			: iterable(obj), predicate(predicate) {}
		virtual ~Filter() = default;
};

class FilterIterator : public Iterator
{
	private:
		Interpreter &context;
		Filter &filter;
		IteratorAdapter it;

		Value value;
		bool consumed = true;

		std::shared_ptr<LiteralExpr> predicate_arg;
		ExprList predicate_arglist;

		inline void next_value()
		{
			if (!consumed)
				return;

			auto &arg = predicate_arg->raw_ref();
			while (it.has_next()) {
				arg = it.next();
				if (context.invoke(filter.predicate, predicate_arglist).get<Boolean>()) {
					value = arg;
					consumed = false;
					return;
				}
			}
			value.clear();
		}

		inline const Value& consume()
		{
			if (value.empty())
				throw IllegalOperationException();

			consumed = true;
			return value;
		}

	public:
		FilterIterator(Filter&, Interpreter&);
		Value next() override;
		bool can_skip(Int n) override;
		Value skip(Int n) override;
		bool has_next() override;
		Value remove() override;
		Value peek() override;
};

}

NATIVE_CLASS(Filter, native::Filter)
NATIVE_CLASS(FilterIterator, native::FilterIterator)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_FILTER_H_ */
