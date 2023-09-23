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

#include <util/util.h>
#include <util/global.h>
#include <interpreter/Interpreter.h>
#include <expression/LiteralExpr.h>
#include <expression/Expression.h>
#include <interpreter/Interpreter.h>
#include <structure/Function.h>
#include <oop/Object.h>
#include <structure/Value.h>
#include <lang/core/File.h>

namespace mtl
{

namespace fs = std::filesystem;

File::File(Interpreter &context) : Class(context, "File")
{
	/* file = new: File("path/to/file.ext") */
	register_method(Methods::Constructor, [&](Args &args) {
		set_path(args);
		return Value::NO_VALUE;
	});

	/* Called automatically when converting to string */
	register_method(Methods::ToString, [&](Args &args) {
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
		Value func = args[1]->evaluate(context);
		Function &action = func.get<Function>();

		if (!fs::is_directory(root_path))
			throw std::runtime_error("File.for_each can only be performed on a directory");

		auto path = std::make_shared<LiteralExpr>();
		ExprList action_args {path};
		for(auto& entry: fs::recursive_directory_iterator(root_path)) {
			path->raw_ref() = entry.path().string();
			context.invoke(action, action_args);
		}

		return Value::NO_VALUE;
	});

	/* file.extension$() */
	register_method("extension", [&](Args &args) {
		return context.make<String>(fs::path(path(args)).extension().string());
	});

	/* file.size$() */
	register_method("size", [&](Args &args) {
		return (Int)fs::file_size(path(args));
	});

	/* file.absolute_path$() */
	register_method("absolute_path", [&](Args &args) {
		return context.make<String>(core::absolute_path(context, path(args)));
	});

	/* file.absolute$() */
	register_method("absolute", [&](Args &args) {
		auto file_path = context.make<String>(core::absolute_path(context, path(args)));
		return context.make<File>(file_path);
	});

	/* file.path$() */
	register_method("path", [&](Args &args) {
		return Object::get_this(args).field(FNAME);
	});

	/* file.filename$() */
	register_method("filename", [&](Args &args) {
		return context.make<String>(fs::path(path(args)).filename().string());
	});

	/* file.parent_path() */
	register_method("parent_path", [&](Args &args) {
		return context.make<String>(fs::path(path(args)).parent_path().string());
	});

	/* file.parent() */
	register_method("parent", [&](Args &args) {
		auto parent_path = context.make<String>(fs::path(path(args)).parent_path().string());
		return context.make<File>(parent_path);
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
		std::string rhs = *mtl::str(args[1]->evaluate(context));
		return fs::equivalent(file, rhs);
	});

	/* file.copy_to$(dest_path) */
	register_method("copy_to", [&](Args &args) {
		std::string from = path(args);
		std::string to = *mtl::str(args[1]->evaluate(context));
		fs::copy(from, to);
		return Value::NO_VALUE;
	});

	/* file.rename$(new_path) */
	register_method("rename", [&](Args &args) {
		fs::rename(path(args), std::string(*mtl::str(args[1]->evaluate(context))));
		return Value::NO_VALUE;
	});

	/* file.remove$() */
	register_method("remove", [&](Args &args) {
		return fs::remove_all(path(args));
	});

	/* ***** Read/Write Operations ***** */

	/* file.read_buffer() */
	register_method("read_buffer", [&](Args &args) {
		return read_contents<Buffer>(path(args));
	});

	/* file.write_buffer(Buffer) */
	register_method("write_buffer", [&](Args &args) {
		auto buffer_v = args[1]->evaluate(context);
		write_contents<Buffer>(path(args), buffer_v);
		return Value::NO_VALUE;
	});

	/* file.read(Buffer, [length = Buffer.size()]) */
	register_method("read", [&](Args &args) {
		auto buffer = args[1]->evaluate(context);
		Int max_bytes;
		if (args.size() >= 3) {
			max_bytes = mtl::num(args[2]->evaluate(context));
		} else {
			max_bytes = buffer.get<Buffer>()->size();
		}
		return read_buffer(Object::get_this(args), buffer, max_bytes);
	});

	/* file.write(Buffer) */
	register_method("write", [&](Args &args) {
		auto buffer = args[1]->evaluate(context);
		write_buffer(Object::get_this(args), buffer);
		return Value::NO_VALUE;
	});

	/* file.read_contents$() */
	register_method("read_contents", [&](Args &args) {
		return read_contents<String>(path(args));
	});

	/* file.write_contents$(str) */
	register_method("write_contents", [&](Args &args) {
		auto buffer_v = args[1]->evaluate(context);
		write_contents<String>(path(args), buffer_v);
		return Value::NO_VALUE;
	});

	/* file.read_line$() */
	register_method("read_line", [&](Args &args) {
		return read_line(Object::get_this(args));
	});

	/* file.write_line$(expr) */
	register_method("write_line", [&](Args &args) {
		write_line(Object::get_this(args), *mtl::str(args[1]->evaluate(context)));
		return Value::NO_VALUE;
	});

	register_method("reset", [&](Args &args) {
		reset(managed_file(Object::get_this(args)));
		return Value::NO_VALUE;
	});

	/* ***** Static methods ***** */

	/* File@cwd$() */
	register_method("cwd", [&](Args &args) {
		return context.make<String>(fs::current_path().string());
	});

	/* file.cd$() */
	register_method("cd", [&](Args &args) {
		fs::current_path(path(args));
		return Value::NO_VALUE;
	});
}

void File::set_path(Args &args)
{
	auto &obj = Object::get_this(args);
	auto path = core::path(context, *mtl::str(args[1]->evaluate(context)));
	obj.def(FNAME) = context.make<String>(path);
}

void File::reset(std::fstream &file)
{
	file.clear();
	file.seekg(0, std::ios::beg);
}

Value File::read_line(Object &obj)
{
	auto line = context.make<String>();
	auto &file = managed_file(obj);
	std::getline(file, *line.get<String>());
	obj.field(IS_EOF) = file.peek() == EOF;
	return line;
}

void File::write_line(Object &obj, const std::string &line)
{
	managed_file(obj) << line << std::endl;
}

Int File::read_buffer(Object &obj, Value out_buffer, Int max_bytes)
{
	auto &buffer = out_buffer.get<Buffer>();
	if (buffer->size() < static_cast<size_t>(max_bytes)) {
		buffer->resize(max_bytes);
	}

	auto &file = managed_file(obj);
	obj.def(IS_EOF) = !!file.read(reinterpret_cast<char*>(buffer->data()), max_bytes);
	return file.gcount();

}

void File::write_buffer(Object &obj, Value in_buffer)
{
	auto &buffer = in_buffer.get<Buffer>();
	auto &file = managed_file(obj);
	file.write(reinterpret_cast<char*>(buffer->data()), buffer->size());
}

std::string File::path(Args &args)
{
	/* This allows to use all `File` methods statically by providing the `path` as the last argument */
	if (Class::static_call(args)) {
		Value p = args[1]->evaluate(context);
//		args.erase(std::next(args.begin()));
		return core::path(context, p.get<String>());
	}

	return Object::get_this(args).field(FNAME).get<String>();
}

bool File::open(Object &obj)
{
	auto file = std::make_shared<std::fstream>(*mtl::str(obj.field(FNAME)));
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

} /* namespace mtl */
