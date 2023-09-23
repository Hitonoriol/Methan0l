#ifndef SRC_LANG_CORE_CLASS_GENERICCONTAINER_H_
#define SRC_LANG_CORE_CLASS_GENERICCONTAINER_H_

#include "Collection.h"
#include "DefaultIterator.h"

#include <oop/NativeClass.h>
#include <structure/Wrapper.h>
#include <util/string.h>

namespace mtl
{

namespace native
{

template<typename Container>
class GenericContainer : public ContainerWrapper<Container>, public Collection
{
	public:
		using ContainerWrapper<Container>::ContainerWrapper;

		~GenericContainer() = default;

		inline Value& operator[](UInt idx)
		{
			return (this->contained)[idx];
		}

		inline void push_back(const Value &value)
		{
			this->contained.push_back(value);
		}

		Value add(Value val) override
		{
			this->contained.push_back(val);
			return Value::NO_VALUE;
		}

		Value insert(Int idx, Value val) override
		{
			this->contained.insert(this->contained.begin() + idx, 1, val);
			return Value::NO_VALUE;
		}

		Value remove_at(UInt idx) override
		{
			auto removed = this->contained.at(idx);
			this->contained.erase(this->contained.begin() + idx);
			return removed;
		}

		Value remove(Value val) override
		{
			for (auto it = this->contained.begin(); it != this->contained.end(); ++it) {
				if (*it == val) {
					auto removed = *it;
					this->contained.erase(it);
					return removed;
				}
			}
			return Value::NIL;
		}

		Value get(Int idx) override
		{
			return this->contained.at(idx);
		}

		UInt size() override
		{
			return this->contained.size();
		}

		void resize(UInt new_size) override
		{
			this->contained.resize(new_size);
		}

		void clear() override
		{
			this->contained.clear();
		}

		UInt index_of(Value value) override
		{
			return mtl::index_of(this->contained, value);
		}

		Boolean contains(Value value) override
		{
			return mtl::contains(this->contained, value);
		}

		Value append() override
		{
			auto &elem = this->contained.emplace_back();
			return Value::ref(elem);
		}

		Boolean is_empty() override
		{
			return this->contained.empty();
		}
};

}

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_GENERICCONTAINER_H_ */
