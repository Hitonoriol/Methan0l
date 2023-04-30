#include "Module.h"

#include <filesystem>
#include <boost/dll.hpp>

#include <interpreter/Interpreter.h>
#include <structure/Value.h>
#include <interpreter/Interpreter.h>
#include <util/util.h>
#include <lang/core/File.h>

namespace mtl::core
{

const std::string
	ModuleSymbols::NAME_FIELD = ".module_name",
	ModuleSymbols::REFERENCE_FIELD = ".mref";


void load_module(Interpreter &context, const std::string &path, Unit &unit)
{
	unit.box();
	std::string name = find_module(context, path);

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

std::string find_module(Interpreter &context, const std::string &path_str)
{
	std::filesystem::path path(core::absolute_path(context, path_str));
	const bool exists = std::filesystem::exists(path);
	if (exists) {
		if (!std::filesystem::is_directory(path))
			return path_str;
		else
			return find_module(context, append(path, "/" + path.filename().string()).string());
	}

	if (!exists && !path.has_extension()) {
		if (std::filesystem::exists(append(path, PROGRAM_EXT)))
			return path.string();
		else if (std::filesystem::exists(append(path.assign(path_str), LIBRARY_EXT)))
			return path.string();
	}

	throw std::runtime_error("Couldn't find module: " + path_str);
}

} /* namespace mtl */
