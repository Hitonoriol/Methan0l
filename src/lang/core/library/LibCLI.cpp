#include <lang/core/library/LibCLI.h>

namespace mtl
{

METHAN0L_LIBRARY(LibCLI)

void LibCLI::load()
{
	function("pause_on_exit", [&] (bool value) {
		cli_hook<bool>(CLIHooks::NO_EXIT) = value;
	});
}

} /* namespace mtl */
