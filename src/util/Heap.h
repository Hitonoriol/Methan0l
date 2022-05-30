#ifndef SRC_UTIL_HEAP_H_
#define SRC_UTIL_HEAP_H_

#include "util/debug.h"
#include "util/string.h"
#include <memory_resource>

#define DEFAULT_MEM_CAP 0x100000
#define DEFAULT_MAX_MEM 0x2000000

namespace mtl
{

class Heap: public std::pmr::unsynchronized_pool_resource
{
	private:
		std::unique_ptr<std::pmr::memory_resource> upstream;
		uint64_t in_use = 0, max_mem = DEFAULT_MAX_MEM;

		inline virtual void* do_allocate(std::size_t __bytes, std::size_t __alignment)
		{
			in_use += __bytes;
			if (in_use > max_mem)
				throw std::runtime_error("Out of memory! "
						"Failed to allocate object of size: " + mtl::str(__bytes) + 'b' + NL
						+ "Max allocatable memory: " + mtl::str(max_mem) + "b (" + mtl::str(in_use) + "b would be used)");
			LOG("* [Alloc] " << __bytes << " bytes, mem in use: " << in_use << " bytes");
			return unsynchronized_pool_resource::do_allocate(__bytes, __alignment);
		}

		inline virtual void do_deallocate(void *__p, std::size_t __bytes, std::size_t __alignment)
		{
			in_use -= __bytes;
			LOG("* [Dealloc] " << __bytes << " bytes, mem in use: " << in_use << " bytes");
			return unsynchronized_pool_resource::do_deallocate(__p, __bytes, __alignment);
		}

	public:
		Heap(std::unique_ptr<std::pmr::memory_resource> upstream);

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

		inline static std::unique_ptr<Heap> create(size_t initial_capacity = DEFAULT_MEM_CAP)
		{
			auto initial_buffer = std::pmr::new_delete_resource()->allocate(initial_capacity);
			auto upstream = std::make_unique<std::pmr::monotonic_buffer_resource>(initial_buffer, initial_capacity);
			return std::make_unique<Heap>(std::move(upstream));
		}
};

} /* namespace mtl */

#endif /* SRC_UTIL_HEAP_H_ */
