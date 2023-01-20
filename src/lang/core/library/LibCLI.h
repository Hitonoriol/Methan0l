#ifndef SRC_LANG_CORE_LIBRARY_LIBCLI_H_
#define SRC_LANG_CORE_LIBRARY_LIBCLI_H_

#include <lang/Library.h>
#include <cli/Methan0l.h>

namespace mtl
{

class LibCLI: public Library
{
	private:
		template<typename T>
		T& cli_hook(const std::string &name)
		{
			return *context->get_env_var(name).get<T*>();
		}

	public:
		using Library::Library;
		void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_CORE_LIBRARY_LIBCLI_H_ */
