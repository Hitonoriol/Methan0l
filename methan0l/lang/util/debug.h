#ifndef SRC_UTIL_DEBUG_H_
#define SRC_UTIL_DEBUG_H_

#include <methan0l/util/io.h>

#define JOIN(...) __VA_ARGS__

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define OUT(...) mtl::out << JOIN(__VA_ARGS__) << std::endl;
#define MTL_DBG if constexpr (mtl::DEBUG)
#define IFDBG(...) MTL_DBG { JOIN(__VA_ARGS__); }
#define LOG(...) MTL_DBG { OUT(__VA_ARGS__) }
#define TRACE(...) LOG("[]" << __FILE__ << ":" << __LINE__ << "] " << JOIN(__VA_ARGS__))

namespace mtl
{

constexpr bool DEBUG = false;

}


#endif /* SRC_UTIL_DEBUG_H_ */
