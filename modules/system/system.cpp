#include "system.h"

#include <methan0l/core/class/String.h>
#include <methan0l/util/system.h>
#include <stdlib/container/Pair.h>

#include <cstdio>
#ifdef _WIN32
#include <io.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace
{

std::string quote_shell_argument(const std::string &argument)
{
#ifdef _WIN32
	std::string quoted = "\"";
	for (const char character : argument)
	{
		if (character == '"')
			quoted += "\"\"";
		else
			quoted += character;
	}
	return quoted + '"';
#else
	std::string quoted = "'";
	for (const char character : argument)
	{
		if (character == '\'')
			quoted += "'\\''";
		else
			quoted += character;
	}
	return quoted + '\'';
#endif
}

int normalize_exit_code(const int status)
{
#ifdef _WIN32
	return status;
#else
	if (status == -1)
		return status;
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	if (WIFSIGNALED(status))
		return 128 + WTERMSIG(status);
	return status;
#endif
}

mtl::Value process_result(const std::string &command, const bool normalize_status)
{
	mtl::Context module_context(*::context);
	auto out = module_context->make<mtl::native::String>();
	const auto raw_status = mtl::exec(command, out.get<mtl::native::String>());
	const auto status = normalize_status ? normalize_exit_code(raw_status) : raw_status;
	return module_context->make<mtl::Pair>(out, status);
}

}

LOAD_MODULE
{
	function("exec", exec);
	function("run_process", run_process);
	function("is_terminal", is_terminal);
}

mtl::Value exec(const std::string &cmd)
{
	return process_result(cmd, false);
}

mtl::Value run_process(const std::string &executable, const std::string &argument)
{
#ifdef _WIN32
	std::string command = "cmd.exe /D /S /C \""
		+ quote_shell_argument(executable) + ' '
		+ quote_shell_argument(argument) + " < NUL 2>&1\"";
#else
	std::string command = quote_shell_argument(executable) + ' '
		+ quote_shell_argument(argument) + " < /dev/null 2>&1";
#endif
	return process_result(command, true);
}

bool is_terminal()
{
#ifdef _WIN32
	return _isatty(_fileno(stdin)) && _isatty(_fileno(stdout));
#else
	return isatty(fileno(stdin)) && isatty(fileno(stdout));
#endif
}
