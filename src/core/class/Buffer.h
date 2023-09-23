#ifndef SRC_LANG_CORE_LIBRARY_CONTAINER_BUFFER_H_
#define SRC_LANG_CORE_LIBRARY_CONTAINER_BUFFER_H_

#include "Collection.h"
#include "DefaultIterator.h"
#include "GenericContainer.h"

#include <oop/NativeClass.h>
#include <structure/Wrapper.h>

namespace mtl
{

using ByteBuffer = std::vector<uint8_t>;
bool operator== (const uint8_t& rhs, const Value& lhs);

NATIVE_CLASS(BufferIterator, native::DefaultIterator<ByteBuffer>)

namespace native
{

class Buffer : public GenericContainer<ByteBuffer>
{
	public:
		using iterator_type = BufferIterator::bound_class;
		using GenericContainer<wrapped_type>::GenericContainer;

		Int get_integer(Int idx, Int size = -1);
		Float get_float(Int idx, Int size = -1);

		Value add(Value val) override;

		Int add_integer(Int val, Int size = -1);
		Int add_float(Float val, Int size = -1);

		void insert_integer(Int idx, Int val, Int size = -1);
		void insert_float(Int idx, Float val, Int size = -1);

		Value to_string(Context context);
		Int hash_code();

		WRAPPER_EQUALS_COMPARABLE(Buffer)

	private:
		template<typename T>
		T get_as(Int idx, Int size)
		{
			static constexpr size_t max_size = sizeof(T);
			if (size <= 0 || static_cast<size_t>(size) > max_size)
				size = max_size;

			auto cur_len = contained.size();
			if (cur_len < static_cast<size_t>(idx + size)) {
				throw std::runtime_error("Reading out of buffer's bounds");
			}

			void *bytes = &contained.at(idx);
			T ret = 0;
			std::memcpy(&ret, bytes, size);
			return ret;
		}

		template<typename T>
		void add_as(T val, Int size, Int pos = -1)
		{
			static constexpr size_t max_size = sizeof(T);
			if (size <= 0 || static_cast<size_t>(size) > max_size)
				size = max_size;

			auto cur_len = contained.size();
			contained.resize(cur_len + static_cast<size_t>(size));

			void *bytes;
			if (pos > 0) {
				contained.insert(contained.begin() + pos, size, 0);
				bytes = &contained.at(pos);
			} else {
				bytes = &contained.at(cur_len);
			}

			std::memcpy(bytes, &val, size);
		}
};

}

NATIVE_CLASS(Buffer, native::Buffer)

} /* namespace mtl */

#endif /* SRC_LANG_CORE_LIBRARY_CONTAINER_BUFFER_H_ */
