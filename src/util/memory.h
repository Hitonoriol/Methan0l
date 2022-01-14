#ifndef SRC_UTIL_MEMORY_H_
#define SRC_UTIL_MEMORY_H_

#include <type_traits>

namespace mtl
{

template<typename Base, typename T>
inline bool instanceof(T *ptr)
{
	return std::is_base_of<Base, T>::value || dynamic_cast<const Base*>(ptr) != nullptr;
}

template<typename Base, typename T>
inline bool instanceof(const T &obj)
{
	return instanceof<Base>(&obj);
}

template<typename Base, typename T>
inline bool instanceof(const std::shared_ptr<T> &ptr)
{
	return instanceof<Base>(ptr.get());
}

template<typename Base, typename T, typename F>
inline bool if_instanceof(T &obj, F &&action)
{
	bool result;
	if ((result = instanceof<Base>(obj)))
		action(static_cast<Base&>(obj));
	return result;
}

}

#endif /* SRC_UTIL_MEMORY_H_ */
