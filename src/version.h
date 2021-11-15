#ifndef SRC_VERSION_H_
#define SRC_VERSION_H_

#include <string_view>

namespace mtl
{

static const std::string SELF_NAME = "Methan0l";


constexpr int MAJOR_VERSION = 0;
constexpr int RELEASE_VERSION = 1;
constexpr int MINOR_VERSION = 3;

constexpr int VERSION_CODE = MAJOR_VERSION * 10000 + RELEASE_VERSION * 100 + MINOR_VERSION;

static const std::string VERSION_STR = "v"
		+ std::to_string(MAJOR_VERSION) + "."
		+ std::to_string(RELEASE_VERSION)
		+ (MINOR_VERSION > 0 ? ("." + std::to_string(MINOR_VERSION)) : "");

static const std::string FULL_VERSION_STR = SELF_NAME + " " + VERSION_STR;

constexpr std::string_view get_os()
{
	#ifdef _WIN64
	return "Windows 64-bit";

	#elif __APPLE__ || __MACH__
	return "Mac OSX";

	#elif __linux__
	return "Linux";

	#elif __FreeBSD__
	return "FreeBSD";

	#elif __unix || __unix__
	return "Unix";

	#else
	return "Other OS";

	#endif
}

}

#endif /* SRC_VERSION_H_ */
