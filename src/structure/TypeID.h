#ifndef SRC_STRUCTURE_TYPEID_H_
#define SRC_STRUCTURE_TYPEID_H_

#include <typeinfo>
#include <string_view>

#include <type.h>
#include <util/meta/type_traits.h>

namespace mtl
{

class TypeID
{
	private:
		const std::type_info *type_info;
		std::string_view name;

	public:
		TypeID(const std::type_info &type_info, std::string_view name = "")
			: type_info(&type_info), name(name)
		{
			if (name.empty())
				this->name = type_info.name();
		}

		TypeID() : TypeID(NONE) {}

		TypeID(const TypeID &rhs) : type_info(rhs.type_info), name(rhs.name) {}

		inline TypeID& operator=(const TypeID &rhs)
		{
			type_info = rhs.type_info;
			name = rhs.name;
			return *this;
		}

		inline std::string_view type_name() const
		{
			return name;
		}

		inline Int type_id() const
		{
			return type_info->hash_code();
		}

		template<typename T>
		inline bool is() const
		{
			IF(is_class_binding<T>::value)
				return *type_info == typeid(typename T::bound_class);
			else
				return *type_info == typeid(T);
		}

		inline const std::type_info& operator*()
		{
			return *type_info;
		}

		inline const std::type_info *operator->() const
		{
			return type_info;
		}

		inline bool operator==(const TypeID &rhs) const
		{
			return *type_info == *rhs.type_info;
		}

		inline bool operator!=(const TypeID &rhs) const
		{
			return !operator==(rhs);
		}

		inline bool operator==(const std::type_info &rhs) const
		{
			return *type_info == rhs;
		}

		template<typename C>
		static inline TypeID of(std::string_view name = "")
		{
			return { typeid(C), name };
		}

		static const TypeID NONE;
};

}

#endif /* SRC_STRUCTURE_TYPEID_H_ */
