#ifndef SRC_UTIL_BENCHMARK_H_
#define SRC_UTIL_BENCHMARK_H_

#include <chrono>

#include "type.h"

#define ENABLE_BENCHMARK false

#if ENABLE_BENCHMARK
	#define BENCHMARK_START	auto start = std::chrono::high_resolution_clock::now();
#else
	#define BENCHMARK_START
#endif

#if ENABLE_BENCHMARK
#define BENCHMARK_END \
		auto end = std::chrono::high_resolution_clock::now(); \
		auto elapsed = std::chrono::duration<double, std::milli>(end - start).count(); \
		OUT("[" << __PRETTY_FUNCTION__ << "] executed in " << elapsed << "ms" \
			<< " (x1M = ~" << (elapsed * 1000) << "s)")
#else
	#define BENCHMARK_END
#endif

#endif /* SRC_UTIL_BENCHMARK_H_ */
