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
		CONSTRUCTOR = "construct",
		TO_STRING = "to_string";
};

struct Parameters
{
	static constexpr std::string_view
		THIS = "this",
		SUPER = "super";
};

#endif /* SRC_STRUCTURE_OBJECT_OOPDEFS_H_ */
