#include "Runner.h"

#include <iostream>
#include <string>

#include "Interpreter.h"
#include "structure/Unit.h"
#include "structure/Value.h"
#include "type.h"

namespace mtl
{

int Runner::run_file(mtl::Interpreter &methan0l, int argc, char **argv)
{
	std::string src_name = std::string(argv[1]);
	methan0l.load(methan0l.load_file(src_name));
	methan0l.load_args(argc, argv);
	mtl::Value ret = methan0l.run();

	if (!ret.empty() && !ret.nil())
		try {
			return ret.as<mtl::dec>();
		} catch (...) {
			std::cerr << "[Exit value] " << ret.to_string() << std::endl;
			return -1;
		}

	return 0;
}

} /* namespace mtl */
