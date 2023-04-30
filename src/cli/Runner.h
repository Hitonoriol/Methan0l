#ifndef SRC_RUNNER_H_
#define SRC_RUNNER_H_

#include <interpreter/Interpreter.h>

namespace mtl
{

class Runner
{
	private:
		Interpreter &methan0l;

		int run();
		bool load_file(const std::string &path);

	public:
		Runner(Interpreter &methan0l);
		int run_file(int argc, char **argv, int start_from = 1);
		int run_file(const std::string &path, const std::vector<std::string> &args);
};

} /* namespace mtl */

#endif /* SRC_RUNNER_H_ */
