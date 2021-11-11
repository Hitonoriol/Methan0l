#ifndef SRC_UTIL_MEMORY_H_
#define SRC_UTIL_MEMORY_H_

#include <type_traits>

namespace mtl
{

template<typename Base, typename T>
inline bool instanceof(const T *ptr)
{
	return std::is_base_of<Base, T>::value || dynamic_cast<const Base*>(ptr) != nullptr;
}

template<typename Base, typename T>
inline bool instanceof(const std::shared_ptr<T> &ptr)
{
	return instanceof<Base>(ptr.get());
}

}

#endif /* SRC_UTIL_MEMORY_H_ */
