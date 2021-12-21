#ifndef SRC_LANG_CORE_LIBMODULE_H_
#define SRC_LANG_CORE_LIBMODULE_H_

#include <lang/Library.h>
#include <boost/dll.hpp>

namespace mtl
{

class LibModule: public Library
{
	private:
		static constexpr const char *MODULE_ID = "methan0l_module", *MODULE_ENTRYPOINT = "load";
		static constexpr std::string_view MODULE_EXT = ".so";

		std::vector<boost::dll::shared_library> modules;

	public:
		LibModule(ExprEvaluator *eval) : Library(eval) {}
		virtual void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_LIBMODULE_H_ */
