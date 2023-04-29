#include "LibModule.h"

#include "util/util.h"
#include "structure/Value.h"
#include "interpreter/Interpreter.h"
#include "lang/core/Module.h"
#include "lang/core/Internal.h"

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
		core::load_module(*context, mtl::str(val(rhs)), module);
		core::import(context, module);
		return Value::NO_VALUE;
	}));

}

} /* namespace mtl */
