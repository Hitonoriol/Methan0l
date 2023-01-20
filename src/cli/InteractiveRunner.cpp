#include "InteractiveRunner.h"

#include <iomanip>
#include <deque>
#include <memory>
#include <utility>

#include "cli/Methan0l.h"
#include "parser/Methan0lParser.h"
#include "expression/IdentifierExpr.h"
#include "structure/Unit.h"
#include "structure/Unit.h"
#include "type.h"
#include "util/memory.h"
#include "version.h"
#include "util/memory.h"
#include "util/class_binder.h"

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
				[](auto &runner)
					{
						std::cout << FULL_VERSION_STR << std::endl;
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
						auto path = std::move(runner.next_arg());
						if (runner.load_program(path))
							out << "Successfully loaded " << std::quoted(path) << "."
									<< NL
									<< "Press <Enter> to run the loaded Unit." << NL;
					}
		},
		{ "run <path/to/script.mt0> - load and execute a methan0l program",
				[](auto &runner)
					{
						auto &mt0 = runner.interpreter();
						auto path = std::move(runner.next_arg());
						if (runner.load_program(path)) {
							ValList args;
							args.push_back(path);
							while (runner.has_args())
								args.push_back(std::move(runner.next_arg()));
							mt0.load_args(std::move(args));
							mt0.run();
						}
					}
		},
		{ "cas - toggle CAS mode (print out each expression result after evalution); off by default",
				[](auto &runner)
					{
						bool enabled = runner.toggle_cas_mode();
						std::cout << "CAS mode " << (enabled ? "enabled" : "disabled") << NL;
						runner.cas_print_end();
					}

		},
		{ "cas-reset - reset all previously saved values",
				[](auto &runner)
					{
						runner.reset_cas();
						std::cout << "All previously saved evaluation results have been reset" << NL;
						runner.cas_print_end();
					}

		}
};

InteractiveRunner::InteractiveRunner(Interpreter &methan0l)
	: methan0l(methan0l)
{
	init_env();
	init_commands();
}

void InteractiveRunner::enable_cas_mode(bool value)
{
	cas_mode = value;
	if (!cas_mode)
		reset_cas();
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

bool InteractiveRunner::load_program(const std::string &path)
{
	if (methan0l.load_program(path))
		return true;
	else {
		out << "Failed to load " << std::quoted(path) << " (or the file is empty)." << NL;
		return false;
	}
}

void InteractiveRunner::save_arg(const std::string &arg)
{
	arg_queue.push_front(arg);
}

std::string InteractiveRunner::next_arg()
{
	if (arg_queue.empty())
		throw std::runtime_error("Too few arguments supplied.");

	std::string arg = std::move(arg_queue.back());
	arg_queue.pop_back();
	return arg;
}

/* Format: !!command */
bool InteractiveRunner::process_commands(const std::string &cmd)
{
	if (methan0l.execution_stopped())
		return false;

	if (cmd.size() < cmd_prefix_len + 1
			|| std::string_view(cmd).substr(0, cmd_prefix.size()) != cmd_prefix)
		return false;

	auto toks = split(cmd, " ");
	auto f_cmd = commands.find(std::string_view(toks[0]).substr(cmd_prefix_len));
	for (auto it = std::next(toks.begin()); it != toks.end(); ++it)
		save_arg(*it);

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
			|| last == TokenType::SEMICOLON
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
		if (!instanceof<IdentifierExpr>(expr))
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
		std::cout << out_prefix << result << NL;
		cas_print_end();
	}
}

void InteractiveRunner::start()
{
	std::cout << FULL_VERSION_STR << " on " << get_os() << ".\n"
			<< "Use \"" << cmd_prefix << "help\" to view command list." << std::endl;
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
		else if (!ready && (line.size() == 1 && line[0] == TokenType::BACKSLASH)) {
			ready = true;
			parse();
		}

		if (ready)
			run();
	} while (!methan0l.execution_stopped());
}

} /* namespace mtl */
