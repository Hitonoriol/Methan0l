#ifndef SRC_LANG_CORE_LIBRARY_LIBCLI_H_
#define SRC_LANG_CORE_LIBRARY_LIBCLI_H_

#include <lang/Library.h>
#include <cli/Methan0l.h>
#include <cli/InteractiveRunner.h>

namespace mtl
{

class LibCLI: public Library
{
	private:
		inline InteractiveRunner& interactive_runner()
		{
			return context->get_env_hook<InteractiveRunner>(CLIHooks::INTERACTIVE_RUNNER);
		}

	public:
		using Library::Library;
		void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_LIBRARY_LIBCLI_H_ */
