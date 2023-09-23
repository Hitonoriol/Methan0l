#ifndef SRC_LANG_CORE_CLASS_RANGE_H_
#define SRC_LANG_CORE_CLASS_RANGE_H_

#include "Iterable.h"
#include "String.h"

namespace mtl
{

namespace native
{

template<typename T>
class RangeIterator;

template<typename T>
class Range
{
	private:
		friend class RangeIterator<T>;

		T start;
		T end;
		T step;

	public:
		using iterator_type = RangeIterator<T>;

		Range(T start, T end, T step)
			: start(start), end(end), step(step)
		{
			if ((end <= start && step > 0) || (end >= start && step < 0)
					|| (end > start && step < 0) || (end < start && step > 0) || step == 0)
				throw std::runtime_error("Invalid range: "
						+ mtl::str(start) + ".." + mtl::str(end) + ".." + mtl::str(step));
		}

		inline T get_start()
		{
			return start;
		}

		inline T get_end()
		{
			return end;
		}

		inline T get_step()
		{
			return step;
		}

		Value to_string(Context context)
		{
			return context->make<String>(std::move(
				"Range [" + mtl::str(start) + "; " + mtl::str(end) + "), step = " + mtl::str(step)
			));
		}

		Int hash_code()
		{
			auto n1 = bit_pattern_as<Int>(start);
			auto n2 = bit_pattern_as<Int>(end);
			auto n3 = bit_pattern_as<Int>(step);
			return (n1 ^ ((n2 >> 32) | (n2 << 16))) + n3;
		}

		virtual ~Range() = default;
};

template<typename T>
class RangeIterator : public Iterator
{
	private:
		T n;
		const Range<T> &range;

		void check_bounds()
		{
			if (n >= range.end || n < range.start)
				throw IllegalOperationException();
		}

	public:
		RangeIterator(const Range<T> &range, Interpreter &context)
			: n(range.start), range(range) {}

		Value next() override
		{
			if (!has_next())
				throw IllegalOperationException();

			auto current = n;
			n += range.step;
			return current;
		}

		bool has_next() override
		{
			return n < range.end;
		}

		Value previous() override
		{
			if (!has_previous())
				throw IllegalOperationException();

			auto current = n;
			n -= range.step;
			return current;
		}

		bool has_previous() override
		{
			return n >= range.start;
		}

		Value skip(/* TODO: change to `Value` */ Int skip = 1)
		{
			if (!can_skip(skip))
				throw IllegalOperationException();

			return n += skip;
		}

		bool can_skip(/* TODO: change to `Value` */ Int skip = 1)
		{
			return (n + skip) < range.end;
		}

		void reverse() override
		{
			if (n == range.start)
				n = range.end;

			else if (n == range.end)
				n = range.start;

			else
				throw IllegalOperationException();
		}

		Value peek() override
		{
			check_bounds();
			return n;
		}
};

}

NATIVE_CLASS(IntRange, native::Range<Int>)
NATIVE_CLASS(IntRangeIterator, native::RangeIterator<Int>)
NATIVE_CLASS(FloatRange, native::Range<Float>)
NATIVE_CLASS(FloatRangeIterator, native::RangeIterator<Float>)
METHAN0L_CLASS(Range)

class RangeAdapter : IterableAdapter
{
	public:
		using IterableAdapter::Adapter;

		Value get_start() ADAPTER_METHOD(get_start)
		Value get_end() ADAPTER_METHOD(get_end)
		Value get_step() ADAPTER_METHOD(get_step)
};

#define RANGE_BINDINGS \
	ABSTRACT_METHOD(get_start) \
	ABSTRACT_METHOD(get_end) \
	ABSTRACT_METHOD(get_step)

#define IMPLEMENTS_RANGE \
	IMPLEMENTS(Range) \
	RANGE_BINDINGS \
	BIND_NATIVE_ITERATOR() \
	BIND_CONSTRUCTOR(Float, Float, Float) \
	CONSTRUCTOR_DARGS(1) /* Default step = 1 */

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_RANGE_H_ */
