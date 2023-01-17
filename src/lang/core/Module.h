#ifndef SRC_LANG_CORE_MODULE_H_
#define SRC_LANG_CORE_MODULE_H_

#include "type.h"
#include <boost/dll.hpp>

/* `boost::dll::import` was renamed to `boost::dll::import_symbol`
 * in Boost 1.76 to avoid collision with C++20 `import` keyword */
#if BOOST_VERSION >= 107600
	#define IMPORT boost::dll::import_symbol
#else
	#define IMPORT boost::dll::import
#endif

namespace mtl
{

class Unit;

}

namespace mtl::core
{

struct ModuleSymbols
{
	static constexpr const char
		*ENTRYPOINT = "init_methan0l_module",
		*MAIN = "load";

	static constexpr std::string_view
		FUNC_DEF_PREFIX = "_register_methan0l_func_";

	static const std::string
		NAME_FIELD,
		REFERENCE_FIELD;
};

std::string find_module(Interpreter&, const std::string &path);
void load_module(Interpreter&, const std::string &path, Unit &unit);

} /* namespace mtl */

#endif /* SRC_LANG_CORE_MODULE_H_ */
