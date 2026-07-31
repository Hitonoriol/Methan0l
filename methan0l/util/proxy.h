#ifndef UTIL_PROXY_H_
#define UTIL_PROXY_H_

template<typename T>
class Proxy
{
private:
	T& obj;

public:
	using type = T;

	Proxy(T& obj) : obj(obj) {}

	inline T& contained()
	{
		return obj;
	}

	inline T* operator->()
	{
		return &obj;
	}

	inline T& operator*()
	{
		return obj;
	}
};

#endif // UTIL_PROXY_H_