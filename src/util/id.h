#include <util/meta/type_traits.h>

#include <cstdint>
#include <typeinfo>

namespace mtl
{

template<typename T>
int64_t type_id()
{
	auto& type = typeid(TYPE(T));
	return static_cast<int64_t>(type.hash_code());
}
	
template<PointerType T>
int64_t type_id(T&& val)
{
	auto& type = typeid(*val);
	return static_cast<int64_t>(type.hash_code());
}
	
template<typename T>
int64_t type_id(T&& val)
{
	auto& type = typeid(val);
	return static_cast<int64_t>(type.hash_code());
}

}