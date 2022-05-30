#include <util/Heap.h>

namespace mtl
{

Heap::Heap(std::unique_ptr<std::pmr::memory_resource> upstream)
		: unsynchronized_pool_resource(upstream.get()),
		  upstream(std::move(upstream))
{
}

} /* namespace mtl */
