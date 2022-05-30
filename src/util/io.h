#ifndef SRC_UTIL_IO_H_
#define SRC_UTIL_IO_H_

#include <sstream>
#include <string>

namespace mtl
{

extern std::ostream &out;

std::string read_file(const std::string &name);
std::string read_file(std::istream &file);

}

#endif /* SRC_UTIL_IO_H_ */
