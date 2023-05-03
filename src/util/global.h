#ifndef SRC_UTIL_GLOBAL_H_
#define SRC_UTIL_GLOBAL_H_

#include <string>

namespace mtl
{

extern bool USE_MONOTONIC_BUFFER;
extern bool HEAP_LIMITED;
extern uint64_t HEAP_MEM_CAP, HEAP_MAX_MEM;

struct Paths
{
	static constexpr std::string_view
		MODULE_DIR = "modules";
};

}

#endif /* SRC_UTIL_GLOBAL_H_ */
