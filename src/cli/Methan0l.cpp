#include "Methan0l.h"

#include "interpreter/Interpreter.h"
#include "Runner.h"
#include "InteractiveRunner.h"
#include "util/global.h"
#include "version.h"

#include <iomanip>
#include <charconv>

/* Launch args
 *
 * Format: ./methan0l --arg1=value --arg2 /path/to/script.mt0 arg1 arg2
 *
 * Available args:
 * 		--no-exit
 * 		--max-mem=123
 * 		--initial-mem=123
 * 		--allocate-fully
 * 		--no-heap-limit
 * 		--no-flat-buffer
 * 		--version
 */

int main(int argc, char **argv)
{
	return mtl::cli().run(argc, argv);
}

namespace mtl
{

std::string
	CLIHooks::NO_EXIT(".no_exit"),
	CLIHooks::INTERACTIVE_RUNNER(".interactive");

int cli::run(int argc, char **argv)
{
	auto arg_start = parse_args(argc, argv);
	if (arg_start < 0)
		return 0;

	int return_value = 0;
	{
		Interpreter methan0l(argv[0]);
		init_env(methan0l);

		if (argc - arg_start > 0)
			return_value = Runner::run_file(methan0l, argc, argv, arg_start);
		else
			InteractiveRunner(methan0l).start();
	}

	if (no_exit) {
		std::string _;
		std::getline(std::cin, _);
	}

	return return_value;
}

void cli::init_env(Interpreter &context)
{
	auto &env = context.get_env_table();
	env.set(CLIHooks::NO_EXIT, &(cli::no_exit));
}

bool is_valid_arg(std::string_view arg, std::string_view cstr, bool has_value = true)
{
	return cstr == arg && (!has_value || (has_value && cstr[arg.length()] == '='));
}

bool is_valid_flag(std::string_view arg, std::string_view cstr)
{
	return is_valid_arg(arg, cstr, false);
}

uint64_t get_numeric_arg(std::string_view arg)
{
	uint64_t num = 0;
	auto numvw = arg.substr(arg.find('=') + 1);
	auto result = std::from_chars(numvw.data(), numvw.data() + numvw.size(), num);

	if (result.ec == std::errc::invalid_argument)
		std::cerr << "Invalid numeric argument: " << std::quoted(arg) << ", defaulting to 0" << NL;

	return num;
}

/* Returns the index of the first non-interpreter argument or -1 to stop execution */
int cli::parse_args(int argc, char **argv)
{
	int parsed = 1;
	for (int i = 1; i < argc; ++i) {
		std::string_view arg(argv[i]);
		if (arg[0] != '-')
			break;

		if (is_valid_flag("--no-exit", arg))
			cli::no_exit = true;

		else if (is_valid_arg("--max-mem", arg)) {
			mtl::HEAP_MAX_MEM = get_numeric_arg(arg);
			if (mtl::HEAP_MEM_CAP > mtl::HEAP_MAX_MEM)
				mtl::HEAP_MEM_CAP = mtl::HEAP_MAX_MEM;
		}

		else if (is_valid_arg("--initial-mem", arg))
			mtl::HEAP_MEM_CAP = get_numeric_arg(arg);

		else if (is_valid_flag("--allocate-fully", arg))
			mtl::HEAP_MEM_CAP = mtl::HEAP_MAX_MEM;

		else if (is_valid_flag("--no-heap-limit", arg))
			mtl::HEAP_LIMITED = false;

		else if (is_valid_flag("--no-flat-buffer", arg))
			mtl::USE_MONOTONIC_BUFFER = false;

		else if (is_valid_flag("--version", arg)) {
			std::cout << mtl::FULL_VERSION_STR << NL;
			return -1;
		}

		else
			std::cerr << "Unrecognized argument: " << std::quoted(arg) << NL;

		++parsed;
	}
	return parsed;
}

}
