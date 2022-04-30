#include "File.h"

#include <iterator>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <filesystem>

#include "Interpreter.h"
#include "expression/LiteralExpr.h"
#include "expression/Expression.h"
#include "ExprEvaluator.h"
#include "structure/Function.h"
#include "structure/object/Object.h"
#include "structure/Value.h"
#include "type.h"
#include "util/util.h"
#include "util/global.h"

namespace mtl
{

namespace fs = std::filesystem;

File::File(ExprEvaluator &eval) : Class(eval, "File")
{
	/* file = new: File("path/to/file.ext") */
	register_method(CONSTRUCT, [&](Args &args) {
		set_path(args);
		return Value::NO_VALUE;
	});

	/* Called automatically when converting to string */
	register_method(TO_STRING, [&](Args &args) {
		return Object::get_this(args).field(FNAME);
	});

	/* file.set$(path) */
	register_method("set", [&](Args &args) {
		set_path(args);
		return Object::get_this_v(args);
	});

	/* file.open$() */
	register_method("open", [&](Args &args) {
		return open(Object::get_this(args));
	});

	register_method("is_open", [&](Object &obj) {
		return obj.field(IS_OPEN);
	});

	register_method("is_eof", [&](Object &obj) {
		return obj.field(IS_EOF);
	});

	/* file.close$() */
	register_method("close", [&](Args &args) {
		return close(Object::get_this(args));
	});

	/* file.for_each$(action) */
	register_method("for_each", [&](Args &args) {
		std::string root_path = path(args);
		Value func = args[1]->evaluate(eval);
		Function &action = func.get<Function>();

		if (!fs::is_directory(root_path))
			throw std::runtime_error("File.for_each can only be performed on a directory");

		auto path = std::make_shared<LiteralExpr>();
		ExprList action_args {path};
		for(auto& entry: fs::recursive_directory_iterator(root_path)) {
			path->raw_ref() = entry.path().string();
			eval.invoke(action, action_args);
		}

		return Value::NO_VALUE;
	});

	/* file.extension$() */
	register_method("extension", [&](Args &args) {
		return fs::path(path(args)).extension().string();
	});

	/* file.size$() */
	register_method("size", [&](Args &args) {
		return (dec)fs::file_size(path(args));
	});

	/* file.absolute_path$() */
	register_method("absolute_path", [&](Args &args) {
		return absolute_path(eval, path(args));
	});

	/* file.path$() */
	register_method("path", [&](Args &args) {
		return Object::get_this(args).field(FNAME);
	});

	/* file.filename$() */
	register_method("filename", [&](Args &args) {
		return fs::path(path(args)).filename().string();
	});

	/* file.is_dir$() */
	register_method("is_dir", [&](Args &args) {
		return fs::is_directory(path(args));
	});

	/* file.exists$() */
	register_method("exists", [&](Args &args) {
		return fs::exists(path(args));
	});

	/* file.mkdirs$() */
	register_method("mkdirs", [&](Args &args) {
		return fs::create_directories(path(args));
	});

	/* file.equivalent$(path) */
	register_method("equivalent", [&](Args &args) {
		std::string file = path(args);
		std::string rhs = str(args[1]->evaluate(eval));
		return fs::equivalent(file, rhs);
	});

	/* file.copy_to$(dest_path) */
	register_method("copy_to", [&](Args &args) {
		std::string from = path(args);
		std::string to = str(args[1]->evaluate(eval));
		fs::copy(from, to);
		return Value::NO_VALUE;
	});

	/* file.rename$(new_path) */
	register_method("rename", [&](Args &args) {
		fs::rename(path(args), str(args[1]->evaluate(eval)));
		return Value::NO_VALUE;
	});

	/* file.remove$() */
	register_method("remove", [&](Args &args) {
		return fs::remove_all(path(args));
	});

	/* ***** Read/Write Operations ***** */

	/* file.read_contents$() */
	register_method("read_contents", [&](Args &args) {
		std::string fname = path(args);
		std::ifstream file(fname);
		if (!file.is_open())
			throw std::runtime_error("Failed to open " + fname);
		Value contents(Type::STRING);
		contents.get<std::string>() = std::move(std::string {std::istreambuf_iterator {file}, {}});
		file.close();
		return contents;
	});

	/* file.write_contents$(str) */
	register_method("write_contents", [&](Args &args) {
		auto fname = path(args);
		std::ofstream file(fname, std::ios::trunc);
		file << str(args[1]->evaluate(eval));
		file.close();
		return Value::NO_VALUE;
	});

	/* file.read_line$() */
	register_method("read_line", [&](Args &args) {
		return read_line(Object::get_this(args));
	});

	/* file.write_line$(expr) */
	register_method("write_line", [&](Args &args) {
		write_line(Object::get_this(args), str(args[1]->evaluate(eval)));
		return Value::NO_VALUE;
	});

	register_method("reset", [&](Args &args) {
		reset(managed_file(Object::get_this(args)));
		return Value::NO_VALUE;
	});

	/* ***** Static methods ***** */

	/* File@cwd$() */
	register_method("cwd", [&](Args &args) {
		return fs::current_path().string();
	});

	/* file.cd$() */
	register_method("cd", [&](Args &args) {
		fs::current_path(path(args));
		return Value::NO_VALUE;
	});
}

void File::set_path(ExprList &args)
{
	Object::get_this(args).def(FNAME) = path(eval, mtl::str(args[1]->evaluate(eval)));
}

void File::reset(std::fstream &file)
{
	file.clear();
	file.seekg(0, std::ios::beg);
}

std::string File::read_line(Object &obj)
{
	std::string line;
	auto &file = managed_file(obj);
	std::getline(file, line);
	obj.field(IS_EOF) = file.peek() == EOF;
	return line;
}

void File::write_line(Object &obj, const std::string &line)
{
	managed_file(obj) << line << std::endl;
}

std::string File::path(ExprList &args)
{
	/* This allows to use all `File` methods statically by providing the `path` as the first argument */
	if (Class::static_call(args)) {
		Value p = args[1]->evaluate(eval);
		args.erase(std::next(args.begin()));
		return path(*INTERPRETER, p);
	}

	return str(Object::get_this(args).field(FNAME));
}

bool File::open(Object &obj)
{
	auto file = std::make_shared<std::fstream>(str(obj.field(FNAME)));
	bool open = file->is_open();
	if (bln(obj.def(IS_OPEN) = open)) {
		obj.def(FILE) = file;
		obj.def(IS_EOF) = false;
	}
	return open;
}

bool File::close(Object &obj)
{
	if (!obj.field(IS_OPEN).get<bool>())
		return false;

	obj.field(FILE).get<fhandle>()->close();
	obj.field(FILE).clear();
	obj.field(IS_OPEN) = false;
	return true;
}

std::fstream& File::managed_file(Object &obj)
{
	if (!bln(obj.def(IS_OPEN)))
		throw std::runtime_error("Accessing an unopened file");

	return *obj.field(FILE).get<fhandle>();
}

struct PathPrefix
{
	static constexpr std::string_view
		RUNDIR = "$:",
		SCRDIR = "#:";
};

/*
 * Expands `path aliases`:
 * 		`$:` - expands into interpreter run directory
 * 		`#:` - expands into script run directory
 * And prepends the result to the rest of the `pathstr`
 * Example: `$:/modules/ncurses` becomes: `/path/to/binary/modules/ncurses`
 * Or expands relative paths into absolute ones via the std::filesystem::absolute if no aliases are present in the `pathstr`
 */
std::string File::absolute_path(ExprEvaluator &eval, const std::string &pathstr)
{
	auto alias = std::string_view(pathstr).substr(0, 2);
	std::string retpath = pathstr;
	if (alias == PathPrefix::RUNDIR)
		replace_all(retpath, alias, RUNDIR);
	else if (alias == PathPrefix::SCRDIR) {
		replace_all(retpath, alias, eval.get_scriptdir());
	} else
		retpath = fs::absolute(retpath).string();
	return retpath;
}

std::string File::path(ExprEvaluator &eval, const std::string &pathstr)
{
	auto alias = std::string_view(pathstr).substr(0, 2);
	if (alias == PathPrefix::RUNDIR || alias == PathPrefix::SCRDIR)
		return absolute_path(eval, pathstr);
	return pathstr;
}

} /* namespace mtl */
