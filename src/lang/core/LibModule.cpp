#include "LibModule.h"
#include "structure/Value.h"
#include "ExprEvaluator.h"

#include <boost/dll.hpp>

namespace mtl
{

void LibModule::load()
{
	/* load_module(name) */
	function("load_module", [&](Args args) {
		Value module_v(Type::UNIT);
		Unit &unit = module_v.get<Unit>();
		module_v.get<Unit>().set_persisent(true);
		std::string name = str(args);
		if (name.substr(name.length() - MODULE_EXT.size()) != MODULE_EXT)
			name += MODULE_EXT;

		boost::dll::shared_library module(name);
		eval->enter_scope(unit);
		if (!module.has(MODULE_ID))
			throw std::runtime_error("\"" + name + "\" is not a methan0l module");

		if constexpr (DEBUG) {
			std::cout << "Loading module \"" << name << "\"..." << std::endl;
			for (std::string &symbol : boost::dll::library_info(name).symbols())
				std::cout << "* " << symbol << std::endl;
		}
		//DataTable &table = unit.local();
		if (module.has(MODULE_ENTRYPOINT))
			(boost::dll::import_symbol<void(ExprEvaluator*)>(module, MODULE_ENTRYPOINT))(eval);
		eval->leave_scope();
		modules.push_back(module);
		return module_v;
	});
}

} /* namespace mtl */
