#include "Baker.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>

#include <type.h>
#include <util/io.h>
#include <util/system.h>
#include <core/File.h>
#include <interpreter/Interpreter.h>

namespace fs = std::filesystem;

Baker::Baker(mtl::Interpreter *context)
	: context(context)
{
	add_dependency("$:/libraries", "libraries");
	add_dependency("$:/libmethan0l.so");
}

void Baker::bake()
{
	std::cout << "Baking " << std::quoted(program_name) << "...\n";

	auto full_out_path = out_path + "/" + program_name;
	fs::create_directories(full_out_path);
	if (!fs::exists(full_out_path)) {
		throw std::runtime_error("Output path \"" + full_out_path + "\" is invalid");
	}

	auto cpp_main_path = full_out_path + "/main.cpp";
	std::ofstream out(cpp_main_path);
	if (!out.is_open()) {
		throw std::runtime_error("Output file \"" + cpp_main_path + "\" could not be opened");
	}

	auto cpp_main = std::vformat(out_template, std::make_format_args(main_code));
	out << cpp_main << std::flush;
	out.close();

	auto home_dir = mtl::core::absolute_path(*context, "$:");
	auto lib_dir = home_dir;
	auto lib_name = "libmethan0l.so";
	auto include_dir = home_dir + "/include";
	auto out_executable_path = full_out_path + "/" + program_name;
	auto build_command = std::vformat(build_command_gcc, std::make_format_args(
		include_dir,
		cpp_main_path,
		out_executable_path,
		lib_dir,
		lib_name
	));

	std::cout << "Compiling baked file with g++...\n";
	std::cout << build_command << "\n";

	std::string compilation_out;
	int retcode = mtl::exec(build_command, compilation_out);
	std::cout << compilation_out << "\n";
	std::cout << "Compiler exited with code " << retcode << "\n";

	if (retcode != 0) {
		std::cout << "Baking failed\n";
		return;
	}

	fs::remove(cpp_main_path);

	std::cout << "Copying dependencies..." << "\n";
	const auto copy_options = fs::copy_options::overwrite_existing; //| fs::copy_options::recursive
	for (auto &dep : dependencies) {
		auto dep_out_path = full_out_path + "/" + dep.dst_path + "/";
		if (!fs::exists(dep_out_path)) {
			fs::create_directories(dep_out_path);
		}
		dep_out_path += fs::path(dep.src_path).filename().string();

		fs::copy(dep.src_path, dep_out_path, copy_options);

		std::cout << "Copied " << std::quoted(dep.src_path)
			<< " to " << std::quoted(dep_out_path) << "\n";
	}
	std::cout << "Done baking " << std::quoted(program_name) << "\n";
}

void Baker::set_program_name(const std::string &name)
{
	program_name = name;
}

void Baker::set_main_file(const std::string &path)
{
	auto absolute_path = mtl::core::absolute_path(*context, path);
	main_code = mtl::read_file(absolute_path);
	if (program_name.empty()) {
		auto file = fs::path(absolute_path).filename();
		program_name = file.string();

		if (file.has_extension()) {
			auto ext_len = file.extension().string().size();
			program_name.erase(program_name.size() - ext_len);
		}
	}
}

void Baker::set_main_code(const std::string &code)
{
	main_code = code;
}

void Baker::add_dependency(const std::string &path, const std::string &dep_out_path)
{
	auto absolute_path = mtl::core::absolute_path(*context, path);
	if (fs::is_directory(absolute_path)) {
		for (auto file : fs::recursive_directory_iterator(absolute_path)) {
			if (dep_out_path.empty()) {

			}

			dependencies.emplace_back(
				fs::absolute(file.path()).string(), dep_out_path
			);
		}
	} else {
		dependencies.emplace_back(absolute_path, dep_out_path);
	}
}

void Baker::set_out_path(const std::string &path)
{
	out_path = mtl::core::absolute_path(*context, path);
}
