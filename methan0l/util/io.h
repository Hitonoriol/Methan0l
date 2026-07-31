#ifndef SRC_UTIL_IO_H_
#define SRC_UTIL_IO_H_

#include <sstream>
#include <string>

#include <methan0l/export.h>

namespace mtl
{

extern METHAN0L_API std::ostream &out;
extern METHAN0L_API std::istream &in;

std::string read_file(const std::string &name);
std::string read_file(std::istream &file);

}

#endif /* SRC_UTIL_IO_H_ */
