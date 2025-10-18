#include "Methan0l.h"

#include <iomanip>
#include <charconv>

#include <version.h>
#include <lang/util/global.h>
#include <lang/Methan0l.h>
#include <cli/Runner.h>
#include <cli/InteractiveRunner.h>

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
 * 		--cas
 */

int main(int argc, char **argv)
{
	return mtl::cli().run(argc, argv);
}

namespace mtl
{

std::string
	CLIHooks::NO_EXIT(".no_exit"),
	CLIHooks::CAS_MODE(".cas"),
	CLIHooks::INTERACTIVE_RUNNER(".interactive");

cli::cli()
	: config(std::make_unique<Methan0lConfig>())
{
}

int cli::run(int argc, char **argv)
{
	auto arg_start = parse_args(argc, argv);
	if (arg_start < 0)
		return 0;

	int return_value = 0;
	{
		config->runpath = argv[0];

		auto methan0l = Methan0l::make(std::move(*config));
		init_env(*methan0l);

		if (argc - arg_start > 0)
			return_value = Runner(*methan0l).run_file(argc, argv, arg_start);
		else
			InteractiveRunner(*methan0l).start();
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
	env.set(CLIHooks::CAS_MODE, &(cli::cas));
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

		else if (is_valid_flag("--cas", arg))
			cli::cas = true;

		else if (is_valid_arg("--max-mem", arg)) {
			config->heap_max_capacity = get_numeric_arg(arg);
			if (config->heap_initial_capacity > config->heap_max_capacity) {
				config->heap_initial_capacity = config->heap_max_capacity;
			}
		}

		else if (is_valid_arg("--initial-mem", arg)) {
			config->heap_initial_capacity = get_numeric_arg(arg);
		}

		else if (is_valid_flag("--allocate-fully", arg)) {
			config->heap_initial_capacity = config->heap_max_capacity;
		}

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
