#ifndef SRC_LANG_CORE_CLASS_DEFAULTITERATOR_H_
#define SRC_LANG_CORE_CLASS_DEFAULTITERATOR_H_

#include <lang/core/class/Iterator.h>
#include <util/containers.h>

namespace mtl
{

template<class C>
class DefaultIterator : public Iterator
{
	private:
		using iterator_type = iterator_of<C>;
		using value_type = typename C::value_type;

		C &container;
		iterator_type current_it;

		inline auto last() { return std::prev(std::end(container)); }
		inline auto after_last() { return std::end(container); }
		inline auto first() { return std::begin(container); }
		inline auto before_first() { return std::prev(std::begin(container)); }

		inline void check_bounds()
		{
			if (current_it == before_first() || current_it == after_last())
				throw IllegalOperationException();
		}

	public:
		DefaultIterator(C &container)
			: container(container), current_it(first())
		{
			static_assert(std::is_same_v<value_type, Value>);
		}

		Value peek() override
		{
			check_bounds();
			return Value::ref(*current_it);
		}

		Value remove() override
		{
			check_bounds();
			auto removed = *current_it;
			mtl::remove(container, current_it);
			return removed;
		}

		bool has_next() override
		{
			return current_it != after_last();
		}

		Value next() override
		{
			if (!has_next())
				throw IllegalOperationException();

			return Value::ref(*(current_it++));
		}

		void reverse() override
		{
			if (current_it == first())
				current_it = last();
			else if (current_it == last())
				current_it = first();
			else
				throw std::runtime_error("Iterator can't be reversed mid-operation");
		}

		bool has_previous() override
		{
			return current_it != before_first();
		}

		Value previous() override
		{
			if (!has_previous())
				throw IllegalOperationException();

			return Value::ref(*(current_it--));
		}
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_DEFAULTITERATOR_H_ */
