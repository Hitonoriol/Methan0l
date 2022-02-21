#ifndef SRC_LANG_CORE_LIBMODULE_H_
#define SRC_LANG_CORE_LIBMODULE_H_

#include <lang/Library.h>
#include <boost/dll.hpp>

namespace mtl
{

class Unit;

/* `boost::dll::import` was renamed to `boost::dll::import_symbol`
 * in Boost 1.76 to avoid collision with C++20 `import` keyword */
#if BOOST_VERSION >= 107600
	#define IMPORT boost::dll::import_symbol
#else
	#define IMPORT boost::dll::import
#endif

class LibModule: public Library
{
	private:
		static constexpr const char
		*MODULE_ENTRYPOINT = "init_methan0l_module",
		*MODULE_MAIN = "load";

		static const std::string MODULE_NAME;

		static constexpr std::string_view
		MODULE_EXT = ".so", FUNC_DEF_PREFIX = "_register_methan0l_func_";

		std::unordered_map<std::string, boost::dll::shared_library> modules;

		std::string find_module(const std::string &path);

	public:
		LibModule(ExprEvaluator *eval) : Library(eval) {}
		virtual void load() override;

		void load_module(const std::string &path, Unit &unit);
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_LIBMODULE_H_ */
