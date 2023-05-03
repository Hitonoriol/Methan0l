#ifndef SRC_UTIL_FILESYSTEM_H_
#define SRC_UTIL_FILESYSTEM_H_

#include <filesystem>

namespace mtl
{

inline std::filesystem::path &append(std::filesystem::path &path, const std::string &apx)
{
	return path.assign(path.string() + apx).make_preferred();
}

inline std::filesystem::path concat(const std::filesystem::path &path, const std::string &apx)
{
	return std::filesystem::path(path.string() + apx).make_preferred();
}

}


#endif /* SRC_UTIL_FILESYSTEM_H_ */
