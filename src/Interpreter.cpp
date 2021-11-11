#include "Interpreter.h"

#include <fstream>
#include <utility>

#include "lang/Library.h"
#include "structure/Value.h"
#include "util/util.h"
#include "structure/object/InbuiltType.h"

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

void Interpreter::lex(std::string &code)
{
	parser.get_lexer().parse(code, true);
}

void Interpreter::preserve_data(bool val)
{
	main.set_persisent(val);
}

Unit Interpreter::load_file(std::string path)
{
	std::ifstream src_file(path, std::ios::binary | std::ios::ate);

	if (!src_file.is_open()) {
		std::cerr << "Failed to open file: \"" + path + "\"" << std::endl;
		return Unit();
	}

	Unit unit = load_unit(src_file);
	src_file.close();
	return unit;
}

Unit Interpreter::load_unit(std::istream &codestr)
{
	std::string code = mtl::read_file(codestr);
	return load_unit(code);
}

Unit Interpreter::load_unit(std::string &code)
{
	try {
		parser.load(code);
	} catch (const std::exception &e) {
		std::cerr << "[Syntax error] "
				<< e.what()
				<< " @ line " << parser.get_lexer().next(true).get_line()
				<< std::endl;
		return Unit();
	} catch (...) {
		std::cerr << "[Unknown parsing error]" << std::endl;
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

void Interpreter::load(std::string &code)
{
	load(load_unit(code));
}

void Interpreter::load(const Unit &main)
{
	load_main(this->main = main);
}

Unit& Interpreter::program()
{
	return main;
}

Value Interpreter::run()
{
	if constexpr (DEBUG)
		out << "Running parsed program..." << std::endl;

	if (main.empty()) {
		std::cerr << "Main Unit is empty. Nothing to run." << std::endl;
		return Value::NO_VALUE;
	}

	Value ret;
	try {
		ret = execute(main);
	} catch (const std::exception &e) {
		std::cerr << "[Runtime error] "
				<< e.what()
				<< " @ line " << get_current_expr()->get_line()
				<< std::endl;
		dump_stack();
	} catch (...) {
		std::cerr << "[Unknown runtime error]" << std::endl;
	}

	if constexpr (DEBUG)
		out << "Program execution finished" << std::endl;

	return ret;
}

void Interpreter::size_info()
{
	out << "Core language structures:\n"
			<< "* Value: " << sizeof(Value) << '\n'
			<< "* Reference: " << sizeof(ValueRef) << '\n'
			<< "* String: " << sizeof(std::string) << '\n'
			<< "* Unit: " << sizeof(Unit) << '\n'
			<< "* Function: " << sizeof(Function) << '\n'
			<< "* Object: " << sizeof(Object) << '\n'
			<< "* NoValue: " << sizeof(NoValue) << '\n'
			<< "* Nil: " << sizeof(Nil) << '\n'
			<< std::endl;

	out << "Containers:\n"
			<< "* List: " << sizeof(ValList) << '\n'
			<< "* Map: " << sizeof(ValMap) << '\n'
			<< std::endl;

	out << "Internal structures:\n"
			<< "* Expression: " << sizeof(Expression) << '\n'
			<< "* Token: " << sizeof(Token) << '\n'
			<< "* Library: " << sizeof(Library) << '\n'
			<< "* ObjectType: " << sizeof(ObjectType) << '\n'
			<< "* InbuiltType: " << sizeof(InbuiltType) << '\n'
			<< "* InbuiltFunc: " << sizeof(InbuiltFunc) << '\n'
			<< std::endl;
}

void Interpreter::print_info()
{
	dump_stack();
	out << "Available inbuilt functions:" << std::endl;
	for (auto &func : functions())
		out << "* " << func.first << '\n';
	out << std::endl;
}

} /* namespace mtl */
