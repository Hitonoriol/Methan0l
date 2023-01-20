#ifndef SRC_CLI_METHAN0L_H_
#define SRC_CLI_METHAN0L_H_

#include <string>

namespace mtl
{

class Interpreter;

struct CLIHooks
{
	static std::string NO_EXIT;
};

class cli
{
	private:
		bool no_exit = false;

		void init_env(mtl::Interpreter&);
		int parse_args(int, char**);

	public:
		int run(int argc, char **argv);
};

}

#endif /* SRC_CLI_METHAN0L_H_ */
