#include "File.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <filesystem>

#include "../../expression/LiteralExpr.h"
#include "../../expression/Expression.h"
#include "../../ExprEvaluator.h"
#include "../../structure/Function.h"
#include "../../structure/object/Object.h"
#include "../../structure/Value.h"
#include "../../type.h"
#include "../../util/util.h"

namespace mtl
{

namespace fs = std::filesystem;

File::File(ExprEvaluator &eval) : InbuiltType(eval, "File")
{
	/* file = File.new$("path/to/file.ext") */
	register_method(std::string(CONSTRUCT), [&](auto args) {
		Object &obj = Object::get_this(args);
		obj.field(FNAME) = str(args[1]->evaluate(eval));
		return NO_VALUE;
	});

	/* Called automatically when converting to string */
	register_method(std::string(TO_STRING), [&](auto args) {
		return Object::get_this(args).field(FNAME);
	});

	/* file.set$(path) */
	register_method("set", [&](auto args) {
		Object::get_this(args).field(FNAME) = str(args[1]->evaluate(eval));
		return NO_VALUE;
	});

	/* file.open$() */
	register_method("open", [&](auto args) {
		return Value(open(Object::get_this(args)));
	});

	/* file.close$() */
	register_method("close", [&](auto args) {
		return Value(close(Object::get_this(args)));
	});

	/* file.for_each$(action) */
	register_method("for_each", [&](auto args) {
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

		return NO_VALUE;
	});

	/* file.extension$() */
	register_method("extension", [&](auto args) {
		return Value(fs::path(path(args)).extension().string());
	});

	/* file.size$() */
	register_method("size", [&](auto args) {
		return Value((dec)fs::file_size(path(args)));
	});

	/* file.absolute_path$() */
	register_method("absolute_path", [&](auto args) {
		return Value(fs::absolute(path(args)).string());
	});

	/* file.is_dir$() */
	register_method("is_dir", [&](auto args) {
		return Value(fs::is_directory(path(args)));
	});

	/* file.exists$() */
	register_method("exists", [&](auto args) {
		return Value(fs::exists(path(args)));
	});

	/* file.mkdirs$() */
	register_method("mkdirs", [&](auto args) {
		return Value(fs::create_directories(path(args)));
	});

	/* file.cd$() */
	register_method("cd", [&](auto args) {
		fs::current_path(path(args));
		return NO_VALUE;
	});

	/* file.equivalent$(path) */
	register_method("equivalent", [&](auto args) {
		std::string file = path(args);
		std::string rhs = str(args[1]->evaluate(eval));
		return Value(fs::equivalent(file, rhs));
	});

	/* file.copy_to$(dest_path) */
	register_method("copy_to", [&](auto args) {
		std::string from = path(args);
		std::string to = str(args[1]->evaluate(eval));
		fs::copy(from, to);
		return NO_VALUE;
	});

	/* file.read_contents$() */
	register_method("read_contents", [&](auto args) {
		return Value(read_file(managed_file(Object::get_this(args))));
	});

	/* file.read_line$() */
	register_method("read_line", [&](auto args) {
		return Value(read_line(Object::get_this(args)));
	});

	/* file.write_line$(expr) */
	register_method("write_line", [&](auto args) {
		write_line(Object::get_this(args), str(args[1]->evaluate(eval)));
		return NO_VALUE;
	});
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
	return str(Object::get_this(args).field(FNAME));
}

bool File::open(Object &obj)
{
	auto file = std::make_unique<std::fstream>(str(obj.field(FNAME)));
	bool open = file->is_open();
	if (bln(obj.field(IS_OPEN) = open)) {
		managed_files.emplace(obj.id(), std::move(file));
		obj.field(IS_EOF) = false;
	}
	return open;
}

bool File::close(Object &obj)
{
	size_t id = obj.id();
	auto file = managed_files.find(id);
	if (file == managed_files.end())
		return false;

	file->second->close();
	managed_files.erase(id);
	return true;
}

std::fstream& File::managed_file(Object &obj)
{
	if (!bln(obj.field(IS_OPEN)))
		throw std::runtime_error("Can't access a non-open file");

	return *(managed_files.at(obj.id()));
}

} /* namespace mtl */
