#include "Runner.h"

#include <iostream>
#include <string>

#include "interpreter/Interpreter.h"
#include "structure/Unit.h"
#include "structure/Value.h"
#include "type.h"

namespace mtl
{

Runner::Runner(Interpreter &methan0l)
	:  methan0l(methan0l) {}

bool Runner::load_file(const std::string &path)
{
	if (methan0l.load_program(path))
		return true;
	else {
		out << "Failed to load " << std::quoted(path) << " (or the file is empty)." << NL;
		return false;
	}
}

int Runner::run_file(int argc, char **argv, int start_from)
{
	std::string src_name(argv[start_from]);
	if (load_file(src_name)) {
		methan0l.load_args(argc, argv, start_from);
		return run();
	}

	return -1;
}

int Runner::run_file(const std::string &path, const ValList &args)
{
	if (load_file(path)) {
		methan0l.load_args(args);
		return run();
	}

	return -1;
}

int Runner::run()
{
	auto ret = methan0l.run();
	if (!ret.empty() && !ret.nil())
		try {
			return ret.as<mtl::Int>();
		} catch (...) {
			std::cerr << "[Exit value] " << ret.to_string() << std::endl;
			return -1;
		}

	return 0;
}

} /* namespace mtl */
