#ifndef SRC_STRUCTURE_OBJECT_OOPDEFS_H_
#define SRC_STRUCTURE_OBJECT_OOPDEFS_H_

#include <string_view>
#include <string>

namespace mtl
{

struct Fields
{
	static constexpr std::string_view
		NATIVE_OBJ = ".o";
};

struct Methods
{
	static constexpr std::string_view
		Constructor = "construct",
		ToString = "to_string",
		Copy = "copy",
		Hashcode = "hash_code";
};

struct Parameters
{
	static constexpr std::string_view
		This = "this",
		Super = "super";
};

struct IndexOperator
{
	static const std::string
		Append, Get, Remove,
		Foreach, Slice, Clear, Insert;
};

}

#endif /* SRC_STRUCTURE_OBJECT_OOPDEFS_H_ */
