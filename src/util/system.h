#ifndef SRC_UTIL_SYSTEM_H_
#define SRC_UTIL_SYSTEM_H_

#include <string>

namespace mtl
{

int exec(const std::string &cmd, std::string &output);
std::string get_env(std::string_view name);

}

#endif /* SRC_UTIL_SYSTEM_H_ */
