#include "Interpreter.h"

#include <fstream>
#include <utility>
#include <filesystem>

#include "lang/Library.h"
#include "structure/Value.h"
#include "util/util.h"
#include "structure/object/InbuiltType.h"
#include "expression/TryCatchExpr.h"
#include "util/meta/function_traits.h"

namespace mtl
{

const std::string Interpreter::LAUNCH_ARGS(".argv");
const std::string Interpreter::SCRDIR(".scrdir");
const std::string Interpreter::F_LOAD_FILE(".load");

Interpreter::Interpreter() : ExprEvaluator()
{
	init_inbuilt_funcs();
}

Interpreter::Interpreter(const char *path)
{
	auto rpath = std::filesystem::absolute(path);
	RUNPATH = rpath.string();
	RUNDIR = rpath.parent_path().string();
	INTERPRETER = this;
	init_inbuilt_funcs();
}

void Interpreter::init_inbuilt_funcs()
{
	register_getter("get_runpath", RUNPATH);
	register_getter("get_rundir", RUNDIR);
	register_func("get_launch_args", [&](Args args) {
		return main.local().get(LAUNCH_ARGS);
	});

	register_func(F_LOAD_FILE, member(this, &Interpreter::load_file));
}

void Interpreter::lex(std::string &code)
{
	parser.get_lexer().lex(code, true);
}

bool Interpreter::load()
{
	return try_load([&]() {
		parser.parse_all();
		load(parser.result());
		parser.clear();
		parser.get_lexer().reset();
	});
}

void Interpreter::preserve_data(bool val)
{
	main.set_persisent(val);
}

bool Interpreter::load_program(const std::string &path)
{
	return load(load_file(path));
}

Unit Interpreter::load_file(const std::string &path)
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

bool Interpreter::load(std::istream &codestr)
{
	return load(load_unit(codestr));
}

bool Interpreter::load(std::string &code)
{
	return load(load_unit(code));
}

bool Interpreter::load(const Unit &main)
{
	load_main(this->main = main);
	return !main.empty();
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
	LOG("Running parsed program...")

	if (main.empty()) {
		IFDBG(std::cerr << "Main Unit is empty. Nothing to run." << std::endl)
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

	LOG("Program execution finished")
	return ret;
}

void Interpreter::load_args(int argc, char **argv)
{
	Value list_v(Type::LIST);
	auto &list = list_v.get<ValList>();
	for (int i = 1; i < argc; ++i)
		list.push_back(Value(std::string(argv[i])));

	auto &table = main.local();
	table.set(LAUNCH_ARGS, list_v);
	table.set(SCRDIR, std::filesystem::absolute(argv[0]).parent_path().string());
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
			<< "* ExprEvaluator: " << sizeof(ExprEvaluator) << '\n'
			<< "* Expression: " << sizeof(Expression) << '\n'
			<< "* Token: " << sizeof(Token) << '\n'
			<< "* Library: " << sizeof(Library) << '\n'
			<< "* ObjectType: " << sizeof(ObjectType) << '\n'
			<< "* InbuiltType: " << sizeof(InbuiltType) << '\n'
			<< "* InbuiltFunc: " << sizeof(InbuiltFunc) << '\n'
			<< std::endl;
}

} /* namespace mtl */
