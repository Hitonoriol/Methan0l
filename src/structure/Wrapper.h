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
		using wrapped_type = T;

		Wrapper() {}
		Wrapper(const Wrapper<T> &rhs) : contained(rhs.contained) {}
		Wrapper(const T& other) : contained(other) {}
		Wrapper(T&& other) : contained(std::move(other)) {}
		Wrapper& operator=(const Wrapper& rhs) { contained = rhs.contained; return *this; }
		Wrapper& operator=(const T& other) { contained = other; return *this; }
		Wrapper& operator=(Wrapper&& rhs) { contained = std::move(rhs.contained); return *this; }
		Wrapper& operator=(T&& other) { contained = std::move(other); return *this; }
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

template<typename T>
class ContainerWrapper : public Wrapper<T>
{
	public:
		using Wrapper<T>::Wrapper;

		inline auto begin() { return this->contained.begin(); }
		auto begin() const { return this->contained.begin(); }

		auto end() { return this->contained.end(); }
		auto end() const { return this->contained.end(); }
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_CLASS_WRAPPER_H_ */
