#include <lang/core/library/LibCLI.h>

namespace mtl
{

METHAN0L_LIBRARY(LibCLI)

void LibCLI::load()
{
	function("pause_on_exit", [&] (bool value) {
		cli_hook<bool>(CLIHooks::NO_EXIT) = value;
	});

	function("enable_cas_mode", [&] (bool value) {
		interactive_runner().enable_cas_mode(value);
		return value;
	});
}

} /* namespace mtl */
