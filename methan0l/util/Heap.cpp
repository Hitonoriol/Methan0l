#include "Heap.h"

namespace mtl
{

Heap::Heap(std::unique_ptr<std::pmr::memory_resource> upstream, uint64_t max_capacity)
	: upstream(std::move(upstream))
	, pool(this->upstream.get())
	, max_mem(max_capacity)
{
}

Heap::~Heap()
{
	if (std::pmr::get_default_resource() == this)
		std::pmr::set_default_resource(std::pmr::new_delete_resource());
}

void* Heap::do_allocate(std::size_t __bytes, std::size_t __alignment)
{
	in_use += __bytes;
	if (max_mem && in_use > max_mem)
		throw std::runtime_error("Out of memory! "
				"Failed to allocate object of size: " + mtl::str(__bytes) + "b. "
				+ " Max allocatable memory: " + mtl::str(max_mem) + "b (" + mtl::str(in_use) + "b would be used)");
	return pool.allocate(__bytes, __alignment);
}

void Heap::do_deallocate(void *__p, std::size_t __bytes, std::size_t __alignment)
{
	in_use -= __bytes;
	return pool.deallocate(__p, __bytes, __alignment);
}

bool Heap::do_is_equal(const std::pmr::memory_resource &__other) const noexcept
{
	return this == &__other || &pool == &__other || upstream.get() == &__other;
}

std::unique_ptr<Heap> Heap::create(uint64_t initial_capacity, uint64_t max_capacity)
{
	auto initial_buffer = std::pmr::new_delete_resource()->allocate(initial_capacity);
	auto upstream = std::make_unique<std::pmr::monotonic_buffer_resource>(initial_buffer, initial_capacity);
	return std::make_unique<Heap>(std::move(upstream), max_capacity);
}

} /* namespace mtl */
