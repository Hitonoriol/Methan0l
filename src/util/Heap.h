#ifndef SRC_UTIL_HEAP_H_
#define SRC_UTIL_HEAP_H_

#include <util/string.h>
#include <lang/util/global.h>

#include <memory_resource>
#include <memory>

namespace mtl
{

class Heap : public std::pmr::memory_resource
{
	private:
		std::unique_ptr<std::pmr::memory_resource> upstream;
		std::pmr::unsynchronized_pool_resource pool;
		uint64_t in_use = 0;
		uint64_t max_mem = 0;

		void* do_allocate(std::size_t __bytes, std::size_t __alignment) override;
		void do_deallocate(void *__p, std::size_t __bytes, std::size_t __alignment) override;
		bool do_is_equal(const std::pmr::memory_resource &__other) const noexcept override;

	public:
		Heap(std::unique_ptr<std::pmr::memory_resource> upstream, uint64_t max_capacity = 0);
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

		static std::unique_ptr<Heap> create(uint64_t initial_capacity, uint64_t max_capacity = 0);
};

} /* namespace mtl */

#endif /* SRC_UTIL_HEAP_H_ */
