#ifndef SRC_UTIL_FILESYSTEM_H_
#define SRC_UTIL_FILESYSTEM_H_

#include <filesystem>

namespace mtl
{

inline std::filesystem::path &append(std::filesystem::path &path, const std::string &apx)
{
	return path.assign(path.string() + apx).make_preferred();
}

inline std::filesystem::path &append(std::filesystem::path &path, std::string_view apx)
{
	return append(path, std::string(apx));
}

}


#endif /* SRC_UTIL_FILESYSTEM_H_ */
