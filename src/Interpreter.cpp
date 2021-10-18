#include "Interpreter.h"

#include <fstream>

namespace mtl
{

Interpreter::Interpreter() : ExprEvaluator()
{
	inbuilt_func("load", [&](Args args) {
		return Value(load_file(eval(args[0]).as<std::string>()));
	});
}

Interpreter::Interpreter(std::string code) : Interpreter()
{
	load(code);
}

void Interpreter::preserve_data(bool val)
{
	main.set_persisent(val);
}

Unit Interpreter::load_file(std::string path)
{
	std::ifstream src_file(path, std::ios::binary | std::ios::ate);

	if (!src_file.is_open())
		throw std::runtime_error("Failed to open file: \"" + path + "\"");

	Unit unit = load_unit(src_file);
	src_file.close();
	return unit;
}

Unit Interpreter::load_unit(std::istream &codestr)
{
	return load_unit(mtl::read_file(codestr));
}

Unit Interpreter::load_unit(const std::string &code)
{
	try {
		parser.load(code);
	} catch (std::exception &e) {
		std::cerr << "[Parsing error] " << e.what() << std::endl;
		return Unit();
	}
	Unit unit = parser.result();
	parser.clear();
	return unit;
}

void Interpreter::load(std::istream &codestr)
{
	load(load_unit(codestr));
}

void Interpreter::load(const std::string &code)
{
	load(load_unit(code));
}

void Interpreter::load(const Unit &main)
{
	load_main(this->main = main);
}

Unit Interpreter::program()
{
	return main;
}

Value Interpreter::run()
{
	if constexpr (DEBUG)
		std::cout << "Running parsed program..." << std::endl;

	if (main.empty()) {
		std::cerr << "Main Unit is empty. Nothing to run." << std::endl;
		return NO_VALUE;
	}

	Value ret;
	try {
		ret = execute(main);
	} catch (std::runtime_error &e) {
		std::cerr << "[Runtime error] " << e.what() << std::endl;
		dump_stack();
	}

	return ret;
}

void Interpreter::print_info()
{
	dump_stack();
	std::cout << "Available inbuilt functions:" << std::endl;
	for (auto func : functions())
		std::cout << "* " << func.first << '\n';
	std::cout << std::endl;
}

} /* namespace mtl */
