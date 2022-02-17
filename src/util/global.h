#ifndef SRC_UTIL_GLOBAL_H_
#define SRC_UTIL_GLOBAL_H_

#include <string>

namespace mtl
{

class Interpreter;

/* Main Interpreter instance */
extern Interpreter *INTERPRETER;

/* Paths to the interpreter executable & the directory it's located in */
extern std::string RUNPATH, RUNDIR;

}

#endif /* SRC_UTIL_GLOBAL_H_ */
