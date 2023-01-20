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
		template<typename T>
		T& cli_hook(const std::string &name)
		{
			auto &hook_var = context->get_env_var(name);
			if (hook_var.nil())
				throw std::runtime_error("No such CLI hook: " + name);
			return *hook_var.get<T*>();
		}

		inline InteractiveRunner& interactive_runner()
		{
			return cli_hook<InteractiveRunner>(CLIHooks::INTERACTIVE_RUNNER);
		}

	public:
		using Library::Library;
		void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_LIBRARY_LIBCLI_H_ */
