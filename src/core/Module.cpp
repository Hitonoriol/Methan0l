#include "Module.h"

#include <filesystem>
#include <boost/dll.hpp>

#include <interpreter/Interpreter.h>
#include <structure/Value.h>
#include <interpreter/Interpreter.h>
#include <util/util.h>
#include <core/File.h>

namespace mtl::core
{

namespace fs = std::filesystem;

const std::string
	ModuleSymbols::NAME_FIELD = ".module_name",
	ModuleSymbols::REFERENCE_FIELD = ".mref";


void load_module(Interpreter &context, const std::string &path, Unit &unit)
{
	unit.box();
	auto name = find_module(context, path);
	if (name.empty())
		throw std::runtime_error("Couldn't find module: " + path);

	/* Load Methan0l src file */
	if (ends_with(name, PROGRAM_EXT)) {
		unit = context.load_file(name);
		return;
	}

	boost::dll::shared_library module(name);
	auto &module_scope = unit.local();
	context.enter_scope(unit);
	if (!module.has(ModuleSymbols::ENTRYPOINT))
		throw std::runtime_error("\"" + name + "\" is not a methan0l module");

	IMPORT<void(Interpreter*)>(module, ModuleSymbols::ENTRYPOINT)(&context);
	for (std::string &symbol : boost::dll::library_info(name).symbols()) {
		if (starts_with(symbol, ModuleSymbols::FUNC_DEF_PREFIX))
			IMPORT<void(void)>(module, symbol)();
	}

	module_scope.set(ModuleSymbols::NAME_FIELD, name);
	if (module.has(ModuleSymbols::MAIN))
		IMPORT<void(void)>(module, ModuleSymbols::MAIN)();

	context.leave_scope();
	if (module.is_loaded())
		module_scope.set(ModuleSymbols::REFERENCE_FIELD, module);
	else
		throw std::runtime_error("Couldn't load module " + name);
}

std::string resolve_module_file(const fs::path &path)
{
	/* Passed path doesn't contain an extension --> try to resolve it
	 * as a Methan0l script first, and if no such script exists, resolve it as native library. */
	if (!path.has_extension()) {
		if (fs::exists(concat(path, str(PROGRAM_EXT))))
			return path.string() + str(PROGRAM_EXT);
		else if (fs::exists(concat(path, str(LIBRARY_EXT))))
			return path.string() + str(LIBRARY_EXT);
	}

	if (fs::exists(path))
		return path.string();

	return {};
}

std::string find_module(Interpreter &context, const std::string &path_str)
{
	fs::path path(core::absolute_path(context, path_str));
	const bool exists = fs::exists(path);

	/* Valid path has been passed */
	if (exists) {
		if (!fs::is_directory(path)) // Extension is specified
			return path_str;
		else
			return find_module(context, concat(path, "/" + path.filename().string()).string());
	}

	auto module_file = resolve_module_file(path);
	if (!module_file.empty())
		return module_file;

	/* Passed path is not valid and is in relative form --> search it
	 * relative to the interpreter home directory. */
	if (fs::path(path_str).is_relative()) {
		fs::path home_modules(context.get_home_dir() + "/" + str(Paths::MODULE_DIR) + "/" + path_str);
		auto home_loc = find_module(context, fs::absolute(home_modules).string());
		if (!home_loc.empty())
			return home_loc;
	}

	return {};
}

} /* namespace mtl */
