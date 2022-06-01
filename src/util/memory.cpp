#include "memory.h"
#include "util.h"

#include <iomanip>

namespace mtl
{

void dump_mem(uint8_t *start, uint64_t bytes)
{
	OUT("Heap dump (" << reinterpret_cast<void*>(start) << "-" << reinterpret_cast<void*>(start + bytes) << ")")
	StringStream addr;
	for (size_t i = 0; i < bytes; ++i) {
		if (i % 16 == 0) {
			out << NL;
			addr << reinterpret_cast<void*>(i) << "-" << reinterpret_cast<void*>(i + 16);
			out << std::setw(21) << addr.str() << " |";
			clear(addr);
		}
		out << std::setw(3) << to_base(start[i], 16);
	}
	out << std::dec << NL;
}

}
