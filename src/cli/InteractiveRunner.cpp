#include "InteractiveRunner.h"

#include <deque>
#include <memory>
#include <utility>

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

const InteractiveRunner::CommandMap InteractiveRunner::commands
{
		{ "help",
				[](auto &runner)
					{
						for (auto&& [cmd, f] : InteractiveRunner::commands)
							std::cout << "* " << cmd << std::endl;
					}
		},
		{ "version",
				[](auto &runner)
					{
						std::cout << FULL_VERSION_STR << std::endl;
					}
		},
		{ "info",
				[](auto &runner)
					{
						runner.interpreter().print_info();
					}
		},
		{ "stack",
				[](auto &runner)
					{
						runner.interpreter().dump_stack();
					}
		},
		{ "program",
				[](auto &runner)
					{
						for (auto &&expr : runner.interpreter().program().expressions())
							std::cout << expr->info() << std::endl;
					}
		},
		{ "load",
				[](auto &runner)
					{
						auto path = std::move(runner.next_arg());
						if (runner.load_program(path))
							out << "Successfully loaded " << std::quoted(path) << "."
									<< NL
									<< "Press <Enter> to run the loaded Unit." << NL;
					}
		},
		{ "run",
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
		}
};

InteractiveRunner::InteractiveRunner(Interpreter &methan0l) : methan0l(methan0l)
{
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

	if (cmd.size() < 3
			|| std::string_view(cmd).substr(0, cmd_prefix.size()) != cmd_prefix)
		return false;

	auto toks = split(cmd, " ");
	auto f_cmd = commands.find(std::string_view(toks[0]).substr(2));
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
	if (main.size() == 1) {
		auto &expr = main.expressions().front();
		if (!instanceof<IdentifierExpr>(expr))
			expr = Expression::return_expr(expr);
	}
	main.clear_result();
}

void InteractiveRunner::run()
{
	auto result = methan0l.run();
	if (result.numeric())
		std::cout << result << std::endl;
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
