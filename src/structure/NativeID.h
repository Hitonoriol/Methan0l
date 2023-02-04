#ifndef SRC_STRUCTURE_NATIVEID_H_
#define SRC_STRUCTURE_NATIVEID_H_

#include <typeinfo>

#include <util/hash.h>

namespace mtl
{

class NativeID
{
	private:
		const std::type_info *type_info;
		std::string_view name;

	public:
		NativeID(const std::type_info &type_info, std::string_view name = "")
			: type_info(&type_info), name(name)
		{
			if (name.empty())
				this->name = type_info.name();
		}

		inline std::string_view type_name() const
		{
			return name;
		}

		inline const std::type_info& operator*()
		{
			return *type_info;
		}

		inline const std::type_info *operator->() const
		{
			return type_info;
		}

		inline bool operator==(const NativeID &rhs) const
		{
			return *type_info == *rhs.type_info;
		}

		template<typename C>
		static inline NativeID of(std::string_view name = "")
		{
			return { typeid(C), name };
		}
};

}

HASH(mtl::NativeID, v, {
	return v->hash_code();
})

#endif /* SRC_STRUCTURE_NATIVEID_H_ */
