#ifndef SRC_LANG_CORE_CLASS_WRAPPER_H_
#define SRC_LANG_CORE_CLASS_WRAPPER_H_

#include <utility>

namespace mtl
{

template<typename T>
class Wrapper
{
	protected:
		T contained;

	public:
		using contained_type = T;

		Wrapper() {}
		Wrapper(const Wrapper<T> &rhs) : contained(rhs.contained) {}
		Wrapper(const T& other) : contained(other) {}
		Wrapper(T&& other) : contained(std::move(other)) {}
		virtual ~Wrapper() = default;

		inline T* operator->()
		{
			return &contained;
		}

		inline T& operator*()
		{
			return contained;
		}

		inline operator T&()
		{
			return contained;
		}
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_WRAPPER_H_ */
