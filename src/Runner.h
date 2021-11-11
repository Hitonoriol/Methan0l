#ifndef SRC_RUNNER_H_
#define SRC_RUNNER_H_

#include "Interpreter.h"

namespace mtl
{

class Runner
{
	public:
		static int run_file(Interpreter&, char *filename);
};

} /* namespace mtl */

#endif /* SRC_RUNNER_H_ */
