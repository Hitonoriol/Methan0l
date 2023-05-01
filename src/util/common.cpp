#include "version.h"

#include <interpreter/Interpreter.h>
#include <util/string.h>

namespace mtl
{

std::string strip_double_space(std::string_view str)
{
	auto pos = str.find("  ");
	if (pos == std::string_view::npos)
		return std::string(str);

	auto res = mtl::str(str.substr(0, pos));
	res += str.substr(pos + 1);
	return res;
}

const std::string SELF_NAME = "Methan0l";

const std::string VERSION_STR(
	"v"
	+ str(MAJOR_VERSION) + "."
	+ str(RELEASE_VERSION)
	+ (MINOR_VERSION != 0 ? "." + str(MINOR_VERSION) : "")
	+ STR(VERSION_SUFFIX)
);

const std::string BUILD_TIMESTAMP(
	strip_double_space(__DATE__) + ", " __TIME__
);

const std::string BUILD_PLATFORM(
	str(get_os()) + " " + str(BITS) + "-bit [" + str(get_architecture()) + "], "
	+ str(BOOST_COMPILER) + ", " + BOOST_STDLIB
);

const std::string FULL_VERSION_STR(
	SELF_NAME + " " + VERSION_STR + " (" + BUILD_TIMESTAMP + ")"
);

bool USE_MONOTONIC_BUFFER = true;
bool HEAP_LIMITED = true;
uint64_t HEAP_MEM_CAP = 0x100000;
uint64_t HEAP_MAX_MEM = 0x2000000;

}
