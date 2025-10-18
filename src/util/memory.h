#ifndef SRC_UTIL_MEMORY_H_
#define SRC_UTIL_MEMORY_H_

#include <type_traits>
#include <memory>
#include <memory_resource>

namespace mtl
{

template <typename Alloc>
struct alloc_deleter
{
	using pointer = typename std::allocator_traits<Alloc>::pointer;

	alloc_deleter(const Alloc& alloc) : alloc(alloc) {}

	void operator()(pointer ptr) const
	{
		Alloc allocCopy(alloc);
		std::allocator_traits<Alloc>::destroy(allocCopy, std::addressof(*ptr));
		std::allocator_traits<Alloc>::deallocate(allocCopy, ptr, 1);
	}

private:
	Alloc alloc;
};

template <typename T, typename Alloc, typename... Args>
auto allocate_unique(const Alloc &alloc, Args &&...args)
{
	using AT = std::allocator_traits<Alloc>;

	static_assert(std::is_same_v<typename AT::value_type, std::remove_cv_t<T>>, "Allocator has the wrong value_type");

	Alloc allocCopy(alloc);
	auto ptr = AT::allocate(allocCopy, 1);

	try {
		AT::construct(allocCopy, std::addressof(*ptr), std::forward<Args>(args)...);
		return std::unique_ptr<T, alloc_deleter<Alloc>>(ptr, alloc_deleter<Alloc>(allocCopy));
	}
	catch (...) {
		AT::deallocate(allocCopy, ptr, 1);
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
