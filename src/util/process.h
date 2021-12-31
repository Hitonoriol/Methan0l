#ifndef SRC_UTIL_PROCESS_H_
#define SRC_UTIL_PROCESS_H_

#include <string>

namespace mtl
{

int exec(const std::string &cmd, std::string &output);

}

#endif /* SRC_UTIL_PROCESS_H_ */
