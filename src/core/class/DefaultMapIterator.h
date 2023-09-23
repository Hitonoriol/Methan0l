#ifndef SRC_LANG_CORE_CLASS_DEFAULTMAPITERATOR_H_
#define SRC_LANG_CORE_CLASS_DEFAULTMAPITERATOR_H_

#include "DefaultIterator.h"

#include <structure/Wrapper.h>

namespace mtl
{

namespace native
{

class MapEntry : public Wrapper<std::pair<Value, Value>>
{
	public:
		using Wrapper<wrapped_type>::Wrapper;
		MapEntry(Value&, Value&);

		void set(Value&, Value&);

		Value key();
		Value value();
};

}

NATIVE_CLASS(MapEntry, native::MapEntry)

namespace native
{

template<typename M>
class DefaultMapIterator : public DefaultIterator<M>
{
	public:
		using iterator_type = typename DefaultIterator<M>::iterator_type;

	protected:
		Value current_pair;

	private:
		inline void populate_pair(iterator_type &it)
		{
			auto &pair = current_pair.get<MapEntry>();
			pair.set(unconst(it->first), it->second);
		}

		inline void populate_pair()
		{
			populate_pair(this->current_it);
		}

	public:
		using DefaultIterator<M>::DefaultIterator;

		DefaultMapIterator(M &map) = delete;

		DefaultMapIterator(M &map, Interpreter &context)
			: DefaultIterator<M>(map)
		{
			current_pair = context.make<mtl::MapEntry>();
			populate_pair();
		}

		Value peek() override
		{
			return current_pair;
		}

		Value remove() override
		{
			auto &context = current_pair.get_context();
			auto removed = *this->current_it;
			auto removed_pair = context.make<mtl::MapEntry>(unconst(removed.first), removed.second);
			this->current_it = this->container.erase(this->current_it);
			return removed_pair;
		}

		Value next() override
		{
			if (!this->has_next())
				throw IllegalOperationException();

			populate_pair();
			++this->current_it;
			return current_pair;
		}

		Value skip(Int n = 1) override
		{
			if (!this->can_skip(n))
				throw IllegalOperationException();

			this->current_it = std::next(this->current_it, n);
			populate_pair();
			return current_pair;
		}
};

} /* namespace native */

NATIVE_CLASS(MapIterator, native::DefaultMapIterator<ValMap>)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_DEFAULTMAPITERATOR_H_ */
