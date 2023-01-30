#ifndef SRC_LANG_CORE_CLASS_DEFAULTITERATOR_H_
#define SRC_LANG_CORE_CLASS_DEFAULTITERATOR_H_

#include <lang/core/class/Iterator.h>

namespace mtl
{

template<class C>
class DefaultIterator : public Iterator
{
	private:
		using iterator_type = decltype(std::begin(std::declval<C&>()));
		using value_type = typename C::value_type;

		C &container;
		iterator_type current_it;

	public:
		DefaultIterator(C &container)
			: container(container), current_it(std::begin(container))
		{
			static_assert(std::is_same_v<value_type, Value>);
		}

		bool has_next() override
		{
			return current_it != std::prev(std::end(container));
		}

		Value next() override
		{
			if (!has_next())
				throw IllegalOperationException();

			return Value::ref(*(current_it++));
		}

		bool has_previous() override
		{
			return current_it != std::prev(std::begin(container));
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
