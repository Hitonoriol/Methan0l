#include "Interpreter.h"
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
 * 		--max-mem=123
 * 		--initial-mem=123
 * 		--allocate-fully
 * 		--no-heap-limit
 * 		--no-flat-buffer
 * 		--version
 */

int parse_args(int, char**);

int main(int argc, char **argv)
{
	auto arg_start = parse_args(argc, argv);
	if (arg_start < 0)
		return 0;

	mtl::Interpreter methan0l(argv[0]);

	if (argc - arg_start > 0)
		return mtl::Runner::run_file(methan0l, argc, argv, arg_start);

	mtl::InteractiveRunner(methan0l).start();
	return 0;
}

bool is_valid_arg(std::string_view arg, const std::string_view &cstr, bool has_value = true)
{
	auto len = arg.length();
	return cstr.substr(0, len) == arg && (!has_value || (has_value && cstr[len] == '='));
}

bool is_valid_flag(std::string_view arg, const std::string_view &cstr)
{
	return is_valid_arg(arg, cstr, false);
}

uint64_t get_numeric_arg(const std::string_view &arg)
{
	uint64_t num = 0;
	auto numvw = arg.substr(arg.find('=') + 1);
	auto result = std::from_chars(numvw.data(), numvw.data() + numvw.size(), num);

	if (result.ec == std::errc::invalid_argument)
		std::cerr << "Invalid numeric argument: " << std::quoted(arg) << ", defaulting to 0" << mtl::NL;

	return num;
}

/* Returns the index of the first non-interpreter argument or -1 to stop execution */
int parse_args(int argc, char **argv)
{
	int parsed = 1;
	for (int i = 1; i < argc; ++i) {
		std::string_view arg(argv[i]);
		if (arg[0] != '-')
			break;

		if (is_valid_arg("--max-mem", arg)) {
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
			std::cout << mtl::FULL_VERSION_STR << mtl::NL;
			return -1;
		}

		else {
			std::cerr << "Unrecognized argument: " << std::quoted(arg) << mtl::NL;
		}

		++parsed;
	}
	return parsed;
}
