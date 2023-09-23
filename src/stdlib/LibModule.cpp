#include "LibModule.h"

#include <util/util.h>
#include <structure/Value.h>
#include <interpreter/Interpreter.h>
#include <core/Module.h>
#include <core/Internal.h>
#include <CoreLibrary.h>

namespace mtl
{

METHAN0L_LIBRARY(LibModule)

void LibModule::load()
{
	/* load(path) */
	function("load", [&](const std::string &path) {
		auto module_v = context->make<Unit>();
		core::load_module(*context, path, module_v.get<Unit>());
		return module_v;
	});

	/* module.unload() */
	function("unload", [&](Unit &unit) {
		Value &name_v = unit.local().get(core::ModuleSymbols::NAME_FIELD);
		if (name_v.nil())
			throw std::runtime_error("Trying to unload a non-module");

		unit.local().clear();
	});

	prefix_operator(TokenType::USING_MODULE, LazyUnaryOpr([&](auto rhs) {
		Unit module;
		core::load_module(*context, *mtl::str(val(rhs)), module);
		if (!module.expressions().empty())
			context->execute(module);
		core::import(context, module);
		return Value::NO_VALUE;
	}));

	/* For testing purposes */
	function("resolve_module", [&](const std::string &path) {
		return str(core::find_module(*context, path));
	});
}

} /* namespace mtl */
