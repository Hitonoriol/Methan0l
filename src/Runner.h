#ifndef SRC_RUNNER_H_
#define SRC_RUNNER_H_

#include "Interpreter.h"

namespace mtl
{

class Runner
{
	public:
		static int run_file(Interpreter&, int argc, char **argv);
};

} /* namespace mtl */

#endif /* SRC_RUNNER_H_ */
