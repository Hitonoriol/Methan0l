#ifndef SRC_UTIL_HEAP_H_
#define SRC_UTIL_HEAP_H_

#include "util/debug.h"
#include "util/string.h"
#include "util/global.h"
#include <memory_resource>

namespace mtl
{

class Heap: public std::pmr::memory_resource
{
	private:
		std::unique_ptr<std::pmr::memory_resource> upstream;
		std::pmr::unsynchronized_pool_resource pool;
		uint64_t in_use = 0, max_mem = HEAP_MAX_MEM;

		void* do_allocate(std::size_t __bytes, std::size_t __alignment) override;
		void do_deallocate(void *__p, std::size_t __bytes, std::size_t __alignment) override;
		bool do_is_equal(const std::pmr::memory_resource &__other) const noexcept override;

	public:
		Heap();
		Heap(std::unique_ptr<std::pmr::memory_resource> upstream);
		~Heap();

		inline uint64_t get_in_use()
		{
			return in_use;
		}

		inline uint64_t get_max_mem()
		{
			return max_mem;
		}

		inline void set_max_mem(uint64_t capacity)
		{
			max_mem = capacity;
		}

		static std::unique_ptr<Heap> create(size_t initial_capacity = HEAP_MEM_CAP);
};

} /* namespace mtl */

#endif /* SRC_UTIL_HEAP_H_ */
