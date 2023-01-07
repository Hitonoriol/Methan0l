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

class Module
{
	private:
		static constexpr const char
		*MODULE_ENTRYPOINT = "init_methan0l_module",
		*MODULE_MAIN = "load";

		static constexpr std::string_view
		MODULE_EXT = ".so", FUNC_DEF_PREFIX = "_register_methan0l_func_";

	public:
		static const std::string MODULE_NAME, MODULE_REFERENCE;

		static std::string find_module(ExprEvaluator&, const std::string &path);
		static void load_module(ExprEvaluator&, const std::string &path, Unit &unit);
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_MODULE_H_ */
