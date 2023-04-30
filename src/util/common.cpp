#include "version.h"

#include <interpreter/Interpreter.h>

namespace mtl
{

const std::string SELF_NAME = "Methan0l";

const std::string VERSION_STR(
		"v"
		+ str(MAJOR_VERSION) + "."
		+ str(RELEASE_VERSION)
		+ (MINOR_VERSION != 0 ? "." + str(MINOR_VERSION) : "")
);

const std::string BUILD_TIMESTAMP(
		str(BUILD_YEAR_CH0) + str(BUILD_YEAR_CH1) + str(BUILD_YEAR_CH2)	+ str(BUILD_YEAR_CH3) + "/"
		+ str(BUILD_MONTH_CH0) + str(BUILD_MONTH_CH1) + "/"
		+ str(BUILD_DAY_CH0) + str(BUILD_DAY_CH1) + ", " __TIME__
);

const std::string FULL_VERSION_STR(
		SELF_NAME + " " + VERSION_STR + " "
		"(#" + str(VERSION_CODE) + ", " + str(BITS) + "-bit, " + BUILD_TIMESTAMP + ")"
);

bool USE_MONOTONIC_BUFFER = true;
bool HEAP_LIMITED = true;
uint64_t HEAP_MEM_CAP = 0x100000;
uint64_t HEAP_MAX_MEM = 0x2000000;

}
