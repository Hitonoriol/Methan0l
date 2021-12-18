#include "Interpreter.h"

#include <fstream>
#include <utility>

#include "lang/Library.h"
#include "structure/Value.h"
#include "util/util.h"
#include "structure/object/InbuiltType.h"
#include "expression/TryCatchExpr.h"

namespace mtl
{

const std::string Interpreter::LAUNCH_ARGS(".argv");

Interpreter::Interpreter() : ExprEvaluator()
{
	inbuilt_func("load", [&](Args args) {
		return Value(load_file(eval(args[0]).as<std::string>()));
	});

	inbuilt_func("get_launch_args", [&](Args args) {
		return main.local().get(LAUNCH_ARGS);
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

void Interpreter::load()
{
	try_load([&]() {
		parser.parse_all();
		load(parser.result());
		parser.clear();
	});
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
	if (!try_load([&]() {parser.load(code);}))
		return Unit();

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

Methan0lParser& Interpreter::get_parser()
{
	return parser;
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
	auto &handler = get_exception_handler();
	do {
		try {
			ret = execute(*current_unit());
		} catch (const std::exception &e) {
			if (!handle_exception(e))
				return -1;
		} catch (Value &e) {
			if (!handle_exception(e))
				return -1;
		} catch (...) {
			std::cerr << "[Unknown runtime error]" << std::endl;
		}
	} while (handler.is_handling());

	if constexpr (DEBUG)
		out << "Program execution finished" << std::endl;

	return ret;
}

void Interpreter::load_args(int argc, char **argv)
{
	Value list_v(Type::LIST);
	auto &list = list_v.get<ValList>();
	for (int i = 1; i < argc; ++i)
		list.push_back(Value(std::string(argv[i])));
	main.local().set(LAUNCH_ARGS, list_v);
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
