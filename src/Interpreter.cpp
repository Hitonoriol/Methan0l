#include "Interpreter.h"

namespace mtl
{

Interpreter::Interpreter(std::string code) : Interpreter()
{
	load_code(code);
}

void Interpreter::load_code(std::string code)
{
	try {
		parser.load(code);
		main = parser.result();
		parser.clear();
	} catch (std::runtime_error &e) {
		std::cerr
		<< "Runtime error during src code parsing: " << e.what() << std::endl;
	}
}

Value Interpreter::run()
{
	Value ret;
	if (main.empty()) {
		std::cerr << "Main Unit is empty. Nothing to run." << std::endl;
		return ret;
	}

	set_main(main);
	try {
		ret = execute(main);
	}
	catch (std::runtime_error &e) {
		std::cerr << "Runtime error: " << e.what() << std::endl;
	}

	return ret;
}

} /* namespace mtl */
