#include <lang/core/Module.h>

#include "interpreter/Interpreter.h"
#include "structure/Value.h"
#include "interpreter/Interpreter.h"
#include "util/util.h"
#include <filesystem>
#include <boost/dll.hpp>
#include <lang/class/File.h>

namespace mtl
{

const std::string
	Module::MODULE_NAME = ".module_name",
	Module::MODULE_REFERENCE = ".mref";


void Module::load_module(Interpreter &eval, const std::string &path, Unit &unit)
{
	unit.box();
	std::string name = find_module(eval, path);

	/* Load Methan0l src file */
	if (ends_with(name, PROGRAM_EXT)) {
		unit = static_cast<Interpreter*>(&eval)->load_file(name);
		return;
	}

	boost::dll::shared_library module(name);
	auto &module_scope = unit.local();
	eval.enter_scope(unit);
	if (!module.has(MODULE_ENTRYPOINT))
		throw std::runtime_error("\"" + name + "\" is not a methan0l module");

	IMPORT<void(Interpreter*)>(module, MODULE_ENTRYPOINT)(&eval);
	for (std::string &symbol : boost::dll::library_info(name).symbols()) {
		if (contains(symbol, FUNC_DEF_PREFIX))
			IMPORT<void(void)>(module, symbol)();
	}

	module_scope.set(MODULE_NAME, name);
	if (module.has(MODULE_MAIN))
		IMPORT<void(void)>(module, MODULE_MAIN)();

	eval.leave_scope();
	if (module.is_loaded())
		module_scope.set(MODULE_REFERENCE, module);
	else
		throw std::runtime_error("Couldn't load module " + name);
}

std::filesystem::path &append(std::filesystem::path &path, const std::string &apx)
{
	return path.assign(path.string() + apx).make_preferred();
}

std::filesystem::path &append(std::filesystem::path &path, std::string_view apx)
{
	return append(path, std::string(apx));
}

std::string Module::find_module(Interpreter &eval, const std::string &path_str)
{
	std::filesystem::path path(File::absolute_path(eval, path_str));
	const bool exists = std::filesystem::exists(path);
	if (exists) {
		if (!std::filesystem::is_directory(path))
			return path_str;
		else
			return find_module(eval, append(path, "/" + path.filename().string()).string());
	}

	if (!exists && !path.has_extension()) {
		if (std::filesystem::exists(append(path, PROGRAM_EXT)))
			return path.string();
		else if (std::filesystem::exists(append(path.assign(path_str), MODULE_EXT)))
			return path.string();
	}

	throw std::runtime_error("Couldn't find module: " + path_str);
}

} /* namespace mtl */
