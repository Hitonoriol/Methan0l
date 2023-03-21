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
		Int id;
		std::string_view name;

	public:
		TypeID(const std::type_info &type_info, std::string_view name = "")
			: id(type_info.hash_code()), name(name)
		{
			if (name.empty())
				this->name = type_info.name();
		}

		TypeID(Int id, std::string_view name)
			: id(id), name(name) {}

		TypeID() : TypeID(NONE) {}

		TypeID(const TypeID &rhs) : id(rhs.id), name(rhs.name) {}

		inline TypeID& operator=(const TypeID &rhs)
		{
			id = rhs.id;
			name = rhs.name;
			return *this;
		}

		inline void set_name(std::string_view name)
		{
			this->name = name;
		}

		inline void set_id(Int id)
		{
			this->id = id;
		}

		inline std::string_view type_name() const
		{
			return name;
		}

		inline Int type_id() const
		{
			return id;
		}

		template<typename T>
		inline bool is() const
		{
			IF(is_class_binding<T>::value)
				return static_cast<size_t>(id) == typeid(typename T::bound_class).hash_code();
			else
				return static_cast<size_t>(id) == typeid(T).hash_code();
		}

		inline bool is_empty()
		{
			return *this == NONE;
		}

		inline bool operator==(const TypeID &rhs) const
		{
			return id == rhs.id;
		}

		inline bool operator!=(const TypeID &rhs) const
		{
			return !operator==(rhs);
		}

		inline bool operator==(const std::type_info &rhs) const
		{
			return static_cast<size_t>(id) == rhs.hash_code();
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
