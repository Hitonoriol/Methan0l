#ifndef SRC_UTIL_MEMORY_H_
#define SRC_UTIL_MEMORY_H_

#include <type_traits>
#include <memory>
#include <memory_resource>

#include <type.h>

namespace mtl
{

class Interpreter;

template <typename T, typename Alloc, typename... Args>
auto allocate_unique(const Alloc &alloc, Args &&...args)
{
	using AT = std::allocator_traits<Alloc>;
	static_assert(std::is_same<typename AT::value_type, std::remove_cv_t<T>>{}(),
				  "Allocator has the wrong value_type");

	Alloc a(alloc);
	auto p = AT::allocate(a, 1);
	try {
		AT::construct(a, std::addressof(*p), std::forward<Args>(args)...);
		using D = alloc_deleter<Alloc>;
		return std::unique_ptr<T, D>(p, D(a));
	}
	catch (...) {
		AT::deallocate(a, p, 1);
		throw;
	}
}

template<typename Base, typename T>
inline bool instanceof(T *ptr)
{
	return (ptr != nullptr)
			&& (std::is_base_of<Base, T>::value || dynamic_cast<const Base*>(ptr) != nullptr);
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
