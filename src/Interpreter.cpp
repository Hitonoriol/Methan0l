#include <structure/object/Class.h>
#include "Interpreter.h"

#include <fstream>
#include <utility>
#include <filesystem>

#include "lang/Library.h"
#include "structure/Value.h"
#include "util/util.h"
#include "expression/TryCatchExpr.h"
#include "util/meta/function_traits.h"

namespace mtl
{

const std::string Interpreter::LAUNCH_ARGS(".argv");
const std::string Interpreter::SCRDIR(".scrdir");
const std::string Interpreter::F_LOAD_FILE(".load");

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

void Interpreter::load_args(int argc, char **argv, int start_from)
{
	ValList list;
	for (int i = start_from; i < argc; ++i)
		list.push_back(Value(std::string(argv[i])));

	load_args(std::move(list));
}

void Interpreter::load_args(ValList &&args)
{
	set_env_globals(args[0]);
	Value list_v(Type::LIST);
	list_v.get<ValList>() = std::move(args);
	main.local().set(LAUNCH_ARGS, list_v);
}

void Interpreter::set_env_globals(const std::string &scrpath)
{
	auto &table = main.local();
	table.set(SCRDIR, std::filesystem::absolute(scrpath).parent_path().string());
}

void Interpreter::print_info()
{
	out << "Core language structures:\n"
			<< "* Value: " << sizeof(Value) << NL
			<< "* Reference: " << sizeof(ValueRef) << NL
			<< "* Object: " << sizeof(Object) << NL
			<< "* String: " << sizeof(std::string) << NL
			<< "* Unit: " << sizeof(Unit) << NL
			<< "* Function: " << sizeof(Function) << NL
			<< "* NoValue: " << sizeof(NoValue) << NL
			<< "* Nil: " << sizeof(Nil) << NL
			<< NL;

	out << "Containers:\n"
			<< "* DataTable: " << sizeof(DataTable) << NL
			<< "* List: " << sizeof(ValList) << NL
			<< "* Map: " << sizeof(ValMap) << NL
			<< NL;

	out << "Internal structures:\n"
			<< "* ExprEvaluator: " << sizeof(ExprEvaluator) << NL
			<< "* Expression: " << sizeof(Expression) << NL
			<< "* Token: " << sizeof(Token) << NL
			<< "* Library: " << sizeof(Library) << NL
			<< "* Class: " << sizeof(Class) << NL
			<< "* InbuiltFunc: " << sizeof(InbuiltFunc) << NL
			<< "* Allocator: " << sizeof(allocator<Value>) << NL
			<< NL;
}

} /* namespace mtl */
