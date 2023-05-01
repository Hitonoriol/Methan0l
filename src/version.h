#ifndef SRC_VERSION_H_
#define SRC_VERSION_H_

#include <string_view>
#include <boost/config.hpp>
#include <boost/predef.h>

#include <util/util.h>

#define MAJOR_VERSION 0
#define RELEASE_VERSION 3
#define MINOR_VERSION 0
#define VERSION_SUFFIX -pre

namespace mtl
{

extern const std::string SELF_NAME;
constexpr int VERSION_CODE = MAJOR_VERSION * 10000 + RELEASE_VERSION * 1000 + MINOR_VERSION;
extern const std::string VERSION_STR, FULL_VERSION_STR;
extern const std::string BUILD_TIMESTAMP;
extern const std::string BUILD_PLATFORM;

constexpr std::string_view WINDOWS("Windows");
constexpr std::string_view get_os()
{
	#ifdef _WIN64
	return WINDOWS;
	#elif _WIN32
	return WINDOWS;

	#elif __APPLE__ || __MACH__
	return "Mac OSX";

	#elif __linux__
	return "Linux";

	#elif __FreeBSD__
	return "FreeBSD";

	#elif __unix || __unix__
	return "Unix";

	#elif __posix
	return "POSIX";

	#else
	return "Other OS";

	#endif
}

constexpr std::string_view get_architecture()
{
#ifdef BOOST_ARCH_X86_64
	return "x86_64";
#elif BOOST_ARCH_X86
	return "x86";
#elif BOOST_ARCH_AARCH64
	return "AArch64";
#elif BOOST_ARCH_ARM
	return "ARM";
#elif BOOST_ARCH_MIPS
	return "MIPS";
#elif BOOST_ARCH_POWERPC
	return "PowerPC";
#else
	return "unknown / unsupported";
#endif
}

constexpr uint8_t get_bits()
{
	return BITS;
}

#define DO_IS_EMPTY_MACRO(VAL) VAL ## 1
#define IS_EMPTY_MACRO(VAL) DO_IS_EMPTY_MACRO(VAL) == 1

constexpr Int get_version_code()
{
	#if IS_EMPTY_MACRO(VERSION_SUFFIX)
		return VERSION_CODE;
	#else
		return -VERSION_CODE;
	#endif
}

}

#endif /* SRC_VERSION_H_ */
