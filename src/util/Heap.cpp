#include <util/Heap.h>

namespace mtl
{

Heap::Heap(std::pmr::memory_resource *upstream)
		: unsynchronized_pool_resource(upstream)
{
}

Heap::Heap(std::unique_ptr<std::pmr::memory_resource> upstream)
		: unsynchronized_pool_resource(upstream.get()),
		  upstream(std::move(upstream))
{
}

void* Heap::do_allocate(std::size_t __bytes, std::size_t __alignment)
{
	in_use += __bytes;
	if (HEAP_LIMITED && in_use > max_mem)
		throw std::runtime_error("Out of memory! "
				"Failed to allocate object of size: " + mtl::str(__bytes) + 'b' + NL
				+ "Max allocatable memory: " + mtl::str(max_mem) + "b (" + mtl::str(in_use) + "b would be used)");
	LOG("* [Alloc] " << __bytes << " bytes, mem in use: " << in_use << " bytes");
	return unsynchronized_pool_resource::do_allocate(__bytes, __alignment);
}

void Heap::do_deallocate(void *__p, std::size_t __bytes, std::size_t __alignment)
{
	in_use -= __bytes;
	LOG("* [Dealloc] " << __bytes << " bytes, mem in use: " << in_use << " bytes");
	return unsynchronized_pool_resource::do_deallocate(__p, __bytes, __alignment);
}

std::unique_ptr<Heap> Heap::create(size_t initial_capacity)
{
	if (!mtl::USE_MONOTONIC_BUFFER)
		return std::make_unique<Heap>(std::pmr::new_delete_resource());

	auto initial_buffer = std::pmr::new_delete_resource()->allocate(initial_capacity);
	auto upstream = std::make_unique<std::pmr::monotonic_buffer_resource>(initial_buffer, initial_capacity);
	return std::make_unique<Heap>(std::move(upstream));
}

} /* namespace mtl */
