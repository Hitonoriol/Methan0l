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

extern bool USE_MONOTONIC_BUFFER;
extern bool HEAP_LIMITED;
extern uint64_t HEAP_MEM_CAP, HEAP_MAX_MEM;

}

#endif /* SRC_UTIL_GLOBAL_H_ */
