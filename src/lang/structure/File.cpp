#include "File.h"

#include <iostream>
#include <fstream>
#include <crtdefs.h>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <filesystem>

#include "../../expression/Expression.h"
#include "../../ExprEvaluator.h"
#include "../../structure/Function.h"
#include "../../structure/object/Object.h"
#include "../../structure/Value.h"
#include "../../type.h"

namespace mtl
{

File::File(ExprEvaluator &eval) : InbuiltType(eval, "File")
{
	/* file = File.new$("path/to/file.ext") */
	register_method(std::string(CONSTRUCT), [&](auto args) {
		Object &obj = Object::get_this(args);
		obj.field(FNAME) = str(args[1]->evaluate(eval));
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

	/* file.size$() */
	register_method("size", [&](auto args) {
		return Value((int)std::filesystem::file_size(str(Object::get_this(args).field(FNAME))));
	});

	/* file.absolute_path$() */
	register_method("absolute_path", [&](auto args) {
		return Value(std::filesystem::absolute(str(Object::get_this(args).field(FNAME))).string());
	});

	/* file.is_dir$() */
	register_method("is_dir", [&](auto args) {
		return Value(std::filesystem::is_directory(str(Object::get_this(args).field(FNAME))));
	});

	/* file.exists$() */
	register_method("exists", [&](auto args) {
		return Value(std::filesystem::exists(str(Object::get_this(args).field(FNAME))));
	});

	/* file.read_line$() */
	register_method("read_line", [&](auto args) {
		return Value(read_line(Object::get_this(args)));
	});

	/* file.write_line$(expr) */
	register_method("write_line", [&](auto args) {
		write_line(Object::get_this(args), str(args[1]->evaluate(eval)));
		return NIL;
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
