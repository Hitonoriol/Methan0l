#ifndef SRC_STRUCTURE_OBJECT_OOPDEFS_H_
#define SRC_STRUCTURE_OBJECT_OOPDEFS_H_

#include <string_view>

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
		Copy = "copy";
};

struct Parameters
{
	static constexpr std::string_view
		This = "this",
		Super = "super";
};

#endif /* SRC_STRUCTURE_OBJECT_OOPDEFS_H_ */
