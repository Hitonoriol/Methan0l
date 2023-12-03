#include "InteractiveRunner.h"

#include "Baker.h"

#include <iomanip>
#include <deque>
#include <memory>
#include <utility>

#include <CoreLibrary.h>
#include <cli/Methan0l.h>
#include <cli/Runner.h>
#include <expression/IdentifierExpr.h>
#include <structure/Unit.h>
#include <util/memory.h>
#include <version.h>
#include <util/memory.h>
#include <util/class_binder.h>
#include <lang/Methan0lParser.h>

namespace mtl
{

const InteractiveRunner::CommandMap InteractiveRunner::default_commands
{
		{ "help - view this menu",
				[](auto &runner)
					{
						for (auto&& entry : runner.help)
							std::cout << " * " << entry << std::endl;
					}
		},
		{ "version - print methan0l version string",
				[](auto&)
					{
						std::cout << VERSION_STR << std::endl;
					}
		},
		{ "info - print info about the interpreter",
				[](auto &runner)
					{
						runner.interpreter().print_info();
					}
		},
		{ "data - dump execution stack and list all variables defined within all scopes "
		  "(will only dump global persistent scope contents in this mode)",
				[](auto &runner)
					{
						runner.interpreter().dump_stack();
						std::cout << std::endl;
					}
		},
		{ "program - print a tree view of the currently loaded program",
				[](auto &runner)
					{
						auto &program = runner.interpreter().program().expressions();
						if (program.empty()) {
							std::cout << "No program is loaded" << std::endl;
							return;
						}

						for (auto &&expr : program)
							std::cout << expr->info() << std::endl;
					}
		},
		{ "load <path/to/script.mt0> - load a methan0l program without executing it",
				[](auto &runner)
					{
						auto path = runner.next_arg();
						if (runner.interpreter().load_program(path))
							out << "Successfully loaded " << std::quoted(path) << "."
									<< NL
									<< "Press <Enter> to run the loaded Unit." << NL;
						else
							out << "Failed to load " << std::quoted(path) << "." << NL;
					}
		},
		{ "run <path/to/script.mt0> [arg1 arg2 ...] - load and execute a methan0l program",
				[](auto &runner)
					{
						auto &mt0 = runner.interpreter();
						auto path = runner.next_arg();

						std::vector<std::string> args;
						args.push_back(path);
						while (runner.has_args())
							args.push_back(runner.next_arg());

						Runner(mt0).run_file(path, args);
					}
		},
		{ "bake [in <script.mt0>] [out <path>] [dep <path>] - bake a methan0l program into a native executable",
				[](auto &runner)
					{
						runner.bake_program();
					}
		},
		{ "cas - toggle CAS mode (print out each expression result after evalution); off by default",
				[](auto &runner)
					{
						runner.toggle_cas_mode();
						runner.toggle_cas_message();
					}

		},
		{ "cas-reset - reset all previously saved values",
				[](auto &runner)
					{
						runner.reset_cas();
						std::cout << "All previously saved evaluation results have been reset" << NL;
						runner.cas_print_end();
					}

		},
		{ "classes - list all registered classes",
				[](auto &runner)
				{
					runner.list_classes();
				}
		},
		{ "class <ClassName> - print info about a class",
				[](auto &runner)
				{
					runner.print_class_info(runner.next_arg());
				}
		},
		{ "env - list all environment variables",
				[](auto &runner)
				{
					runner.print_env_info();
				}
		}
};

InteractiveRunner::InteractiveRunner(Interpreter &methan0l)
	: methan0l(methan0l)
{
	init_env();
	init_commands();
}

std::string InteractiveRunner::cmd_str(std::string_view cmd)
{
	return mtl::str(cmd_prefix) + mtl::str(cmd);
}

void InteractiveRunner::enable_cas_mode(bool value)
{
	cas_mode = value;
	if (!cas_mode)
		reset_cas();
}

void InteractiveRunner::toggle_cas_message()
{
	out << "CAS mode "
			<< (cas_mode ? "enabled" : "disabled")
			<< ". Use " << std::quoted(cmd_str("cas")) << " to toggle it."
			<< NL;
	cas_print_end();
}

void InteractiveRunner::bake_program()
{
	Baker baker(&methan0l);
	while(has_args()) {
		auto arg = next_arg();
		if (arg == "in") {
			baker.set_main_file(next_arg());
		}
		else if (arg == "out") {
			baker.set_out_path(next_arg());
		}
		else if (arg == "dep") {
			baker.add_dependency(next_arg());
		}
	}
	baker.bake();
}

Value InteractiveRunner::get_saved_value(size_t idx)
{
	if (idx == 0)
		return prev_results.back();

	--idx;
	if (idx >= prev_results.size())
		throw std::runtime_error("No such saved value: @" + str(idx));

	return prev_results[idx];
}

void InteractiveRunner::reset_cas()
{
	prev_results.clear();
}

bool InteractiveRunner::toggle_cas_mode()
{
	enable_cas_mode(!cas_mode);
	return cas_mode;
}

void InteractiveRunner::init_env()
{
	auto &env = methan0l.get_env_table();
	env.set(CLIHooks::INTERACTIVE_RUNNER, this);

	if (env.exists(CLIHooks::CAS_MODE))
		enable_cas_mode(methan0l.get_env_hook<bool>(CLIHooks::CAS_MODE));
}

void InteractiveRunner::init_commands()
{
	/* Initialize help menu by separating commands from their descriptions */
	for (auto &[full_name, action] : default_commands) {
		help.push_back(full_name);
		auto space_idx = full_name.find(" ");
		if (space_idx != std::string_view::npos)
			commands.emplace(full_name.substr(0, space_idx), action);
	}
}

std::string InteractiveRunner::next_arg()
{
	if (arg_queue.empty())
		throw std::runtime_error("Too few arguments supplied.");

	std::string arg = std::move(arg_queue.front());
	arg_queue.pop_front();
	return arg;
}

void InteractiveRunner::parse_args(const std::string &cmd)
{
	std::string arg;
	bool parsing_path = false;
	for (auto chr : cmd) {
		// Start of path string
		if (!parsing_path && chr == '\"') {
			parsing_path = true;
			continue;
		}

		// End of path string
		if (parsing_path && chr == '\"') {
			parsing_path = false;
			arg_queue.push_back(arg);
			arg.clear();
			continue;
		}

		// Regular non-path argument
		if (chr == ' ') {
			arg_queue.push_back(arg);
			arg.clear();
		} else {
			arg += chr;
		}
	}
	
	if (!arg.empty()) {
		arg_queue.push_back(arg);
	}
}

/* Format: /command */
bool InteractiveRunner::process_commands(const std::string &cmd)
{
	if (methan0l.execution_stopped())
		return false;

	if (cmd.size() < cmd_prefix_len + 1
			|| std::string_view(cmd).substr(0, cmd_prefix.size()) != cmd_prefix)
		return false;

	parse_args(cmd);
	auto cmd_name = next_arg().substr(cmd_prefix_len);
	auto f_cmd = commands.find(std::string_view{cmd_name});

	if (f_cmd != commands.end()) {
		try {
			f_cmd->second(*this);
		} catch (std::exception &e) {
			std::cerr << "[Interpreter command error] " << e.what() << std::endl;
		} catch (...) {
			std::cerr << "[Unknown interpreter command error]" << std::endl;
		}
		arg_queue.clear();
		return true;
	}

	return false;
}

bool InteractiveRunner::load_line(std::string &line)
{
	auto &lexer = methan0l.get_parser().get_lexer();
	char last = line.back();
	methan0l.lex(line += '\n');

	bool ready = !(lexer.has_unclosed_blocks()
			|| last == Tokens::SEMICOLON
			|| std::isspace(last));

	if (ready)
		parse();

	return ready;
}

void InteractiveRunner::parse()
{
	methan0l.load();
	methan0l.preserve_data(true);
	methan0l.get_parser().clear();
	methan0l.get_parser().get_lexer().reset(true);
	auto &main = methan0l.program();
	/* Wrap single expressions in return expressions */
	if (cas_mode && main.size() == 1) {
		auto &expr = main.expressions().front();
		expr = Expression::return_expr(expr);
	}
	main.clear_result();
}

void InteractiveRunner::run()
{
	auto result = methan0l.run();
	if (cas_mode && !result.empty() && !result.nil()) {
		prev_results.push_back(result);
		auto out_prefix = str(cas_output_pattern);
		StringFormatter(out_prefix, {"[@" + str(prev_results.size()) + "]"}).format();

		std::string result_str;
		if (!result.object() && result.is_callable()) {
			result_str = "<" + std::string(result.type_name()) + ">";
		} else {
			result_str = *result.to_string();
		}

		std::cout << out_prefix << result_str << NL;
		cas_print_end();
	}
}

void InteractiveRunner::start()
{
	std::cout << FULL_VERSION_STR << ". " << NL
		<< "Platform: " << BUILD_PLATFORM << "." << NL
		<< "Use \"" << cmd_prefix << "help\" to view command list." << NL;

	/* Print CAS mode toggle message if started with --cas flag */
	if (cas_mode)
		toggle_cas_message();

	std::string line;
	bool ready = true;
	do {
		std::cout << (ready ? prompt : prompt_multiline) << std::flush;
		getline(std::cin, line);
		if (!line.empty()) {
			if (process_commands(line))
				continue;
			ready = load_line(line);
		}
		/* Force parse all lexed tokens when a single `\` is received */
		else if (!ready && (line.size() == 1 && line[0] == Tokens::BACKSLASH)) {
			ready = true;
			parse();
		}

		if (ready)
			run();
	} while (!methan0l.execution_stopped());
}

void InteractiveRunner::list_classes()
{
	auto classes = interpreter().get_type_mgr().get_classes();
	for (Class *clazz : classes) {
		auto &native = clazz->get_native_id();
		OUT("* "
				<< *clazz
				<< ", native id: {" << native.type_name() << " / "
				<< std::hex << native.type_id() << std::dec
				<< ")")
	}
}

void InteractiveRunner::print_class_info(const std::string &name)
{
	auto types = methan0l.get_type_mgr();
	auto &clazz = types.get_class(name);

	auto &interfaces = clazz.get_interfaces();

	auto it = std::begin(interfaces), end = std::end(interfaces);
	auto int_str = mtl::stringify([&]() {
		if (it == end) return empty_string;
		return (*(it++))->get_name();
	});

	OUT("Class " << clazz.get_name() << std::hex << std::dec)
	OUT("Inherits: " << (clazz.has_superclass() ? clazz.get_superclass()->get_name() : "<Nothing>"))
	OUT("Implements: " << int_str << NL)
	OUT("Fields" << NL << clazz.get_fields() << NL)
	OUT("Methods" << NL << clazz.get_methods())
}

void InteractiveRunner::print_env_info()
{
	auto &table = methan0l.get_env_table();
	out << "Environment variables:" << NL;
	for (auto &[name, value] : table.managed_map())
		out << "* " << name << " = " << value << NL;
}

} /* namespace mtl */
