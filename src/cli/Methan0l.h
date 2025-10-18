#ifndef SRC_CLI_METHAN0L_H_
#define SRC_CLI_METHAN0L_H_

#include <string>
#include <cstdint>
#include <memory>

namespace mtl
{

class Interpreter;
struct Methan0lConfig;

struct CLIHooks
{
	static std::string
		NO_EXIT,
		CAS_MODE,
		INTERACTIVE_RUNNER;
};

class cli
{
public:
	cli();
	int run(int argc, char** argv);

private:
	void init_env(mtl::Interpreter&);
	int parse_args(int, char**);

	std::unique_ptr<Methan0lConfig> config;
	bool no_exit = false;
	bool cas = false;
};

}

#endif /* SRC_CLI_METHAN0L_H_ */
