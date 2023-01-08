#include "LibModule.h"
#include "interpreter/Interpreter.h"
#include "structure/Value.h"
#include "interpreter/ExprEvaluator.h"
#include "util/util.h"
#include <filesystem>
#include <boost/dll.hpp>
#include <lang/class/File.h>
#include <lang/core/Module.h>
#include <lang/core/Internal.h>

#include <lang/core/Internal.h>

namespace mtl
{

void LibModule::load()
{
	/* load(path) */
	function("load", [&](std::string path) {
		Value module_v(Type::UNIT);
		Module::load_module(*eval, path, module_v.get<Unit>());
		return module_v;
	});

	/* module.unload() */
	function("unload", [&](Unit &unit) {
		Value &name_v = unit.local().get(Module::MODULE_NAME);
		if (name_v.nil())
			throw std::runtime_error("Trying to unload a non-module");

		unit.local().clear();
	});

	prefix_operator(TokenType::USING_MODULE, LazyUnaryOpr([&](auto rhs) {
		Unit module;
		Module::load_module(*eval, mtl::str(val(rhs)), module);
		Internal::import(eval, module);
		return Value::NO_VALUE;
	}));

}

} /* namespace mtl */
