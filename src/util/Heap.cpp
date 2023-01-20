#include <util/Heap.h>

namespace mtl
{

Heap::Heap()
	: pool(std::pmr::new_delete_resource()) {}

Heap::Heap(std::unique_ptr<std::pmr::memory_resource> upstream)
	: upstream(std::move(upstream)), pool(this->upstream.get()) {}

Heap::~Heap()
{
	if (std::pmr::get_default_resource() == this)
		std::pmr::set_default_resource(std::pmr::new_delete_resource());
}

void* Heap::do_allocate(std::size_t __bytes, std::size_t __alignment)
{
	in_use += __bytes;
	if (HEAP_LIMITED && in_use > max_mem)
		throw std::runtime_error("Out of memory! "
				"Failed to allocate object of size: " + mtl::str(__bytes) + 'b' + NL
				+ "Max allocatable memory: " + mtl::str(max_mem) + "b (" + mtl::str(in_use) + "b would be used)");
	LOG("* [Alloc] " << __bytes << " bytes, mem in use: " << in_use << " bytes");
	return pool.allocate(__bytes, __alignment);
}

void Heap::do_deallocate(void *__p, std::size_t __bytes, std::size_t __alignment)
{
	in_use -= __bytes;
	LOG("* [Dealloc] " << __bytes << " bytes, mem in use: " << in_use << " bytes");
	return pool.deallocate(__p, __bytes, __alignment);
}

bool Heap::do_is_equal(const std::pmr::memory_resource &__other) const noexcept
{
	return this == &__other || &pool == &__other || upstream.get() == &__other;
}

std::unique_ptr<Heap> Heap::create(size_t initial_capacity)
{
	if (!mtl::USE_MONOTONIC_BUFFER)
		return std::make_unique<Heap>();

	auto initial_buffer = std::pmr::new_delete_resource()->allocate(initial_capacity);
	auto upstream = std::make_unique<std::pmr::monotonic_buffer_resource>(initial_buffer, initial_capacity);
	return std::make_unique<Heap>(std::move(upstream));
}

} /* namespace mtl */
