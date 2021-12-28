#include "LibModule.h"
#include "Interpreter.h"
#include "structure/Value.h"
#include "ExprEvaluator.h"
#include "util/util.h"

#include <filesystem>
#include <boost/dll.hpp>

#include "LibUnit.h"

namespace mtl
{

const std::string LibModule::MODULE_NAME = ".module_name";

void LibModule::load()
{
	/* load(path) */
	eval->register_func("load", [&](std::string path) {
		Value module_v(Type::UNIT);
		load_module(path, module_v.get<Unit>());
		return module_v;
	});

	/* module.unload() */
	eval->register_func("unload", [&](Unit &unit) {
		Value &name_v = unit.local().get(MODULE_NAME);
		auto &name = name_v.get<std::string>();
		if (name_v.nil())
			throw std::runtime_error("Trying to unload a non-module");

		modules.at(name).unload();
		modules.erase(name);
		unit.local().clear();
	});

	prefix_operator(TokenType::USING_MODULE, [&](auto rhs) {
		Unit module;
		load_module(mtl::str(val(rhs)), module);
		LibUnit::import(eval, module);
		return Value::NO_VALUE;
	});

}

void LibModule::load_module(const std::string &path, Unit &unit)
{
	unit.set_persisent(true);
	std::string name = find_module(path);

	/* Load Methan0l src file */
	if (ends_with(name, PROGRAM_EXT)) {
		auto mv = eval->invoke_inbuilt_func(Interpreter::F_LOAD_FILE, { Value::wrapped(name) });
		unit = mv.get<Unit>();
		return;
	}

	boost::dll::shared_library module(name);
	eval->enter_scope(unit);
	if (!module.has(MODULE_ENTRYPOINT))
		throw std::runtime_error("\"" + name + "\" is not a methan0l module");

	boost::dll::import_symbol<void(ExprEvaluator*)>(module, MODULE_ENTRYPOINT)(eval);
	for (std::string &symbol : boost::dll::library_info(name).symbols()) {
		if (contains(symbol, FUNC_DEF_PREFIX))
			boost::dll::import_symbol<void(void)>(module, symbol)();
	}

	unit.local().set(MODULE_NAME, name);
	if (module.has(MODULE_MAIN))
		boost::dll::import_symbol<void(void)>(module, MODULE_MAIN)();

	eval->leave_scope();
	if (module.is_loaded())
		modules.emplace(name, module);
	else
		throw std::runtime_error("Couldn't load module " + name);
}

std::string LibModule::find_module(const std::string &path_str)
{
	std::filesystem::path path(path_str);
	const bool exists = std::filesystem::exists(path);
	if (exists && !std::filesystem::is_directory(path))
		return path_str;

	if (!exists && !path.has_extension()) {
		if (std::filesystem::exists(path.append(PROGRAM_EXT)))
			return path.string();
		else if (std::filesystem::exists(path.assign(path_str).append(MODULE_EXT)))
			return path.string();
	}

	throw std::runtime_error("Couldn't find module: " + path_str);
}

} /* namespace mtl */
