#include "InteractiveRunner.h"

#include <deque>
#include <memory>
#include <utility>

#include "structure/Unit.h"
#include "expression/IdentifierExpr.h"
#include "structure/Unit.h"
#include "type.h"
#include "util/memory.h"
#include "version.h"

namespace mtl
{

const InteractiveRunner::CommandMap InteractiveRunner::commands
{
		{ "help",
				[](auto &runner)
					{
						for (auto &&[cmd, f] : InteractiveRunner::commands)
							std::cout << "* " << cmd << std::endl;
					}
		},
		{ "version",
				[](auto &runner)
					{
						std::cout << FULL_VERSION_STR << std::endl;
					}
		},
		{ "stack",
				[](auto &runner)
					{
						runner.interpreter().dump_stack();
					}
		},
		{ "size_info",
				[](auto &runner)
					{
						runner.interpreter().size_info();
					}
		},
		{ "expressions",
				[](auto &runner)
					{
						for (auto &&expr : runner.interpreter().program().expressions())
							std::cout << "\t" << expr->info() << std::endl;
					}
		},
		{ "load",
				[](auto &runner)
					{
						auto &mt0 = runner.interpreter();
						mt0.load(mt0.load_file(runner.next_arg()));
					}
		}
};

InteractiveRunner::InteractiveRunner(Interpreter &methan0l) : methan0l(methan0l)
{
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
	if (methan0l.force_quit())
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
		methan0l.load();

	return ready;
}

void InteractiveRunner::start()
{
	std::cout << FULL_VERSION_STR << " on " << get_os() << ".\n"
			<< "Use \"" << cmd_prefix << "help\" to view command list." << std::endl;
	methan0l.preserve_data(true);
	std::string line;
	bool ready = true;
	do {
		std::cout << (ready ? prompt : prompt_multiline) << std::flush;
		getline(std::cin, line);
		if (process_commands(line))
			continue;
		ready = load_line(line);
		/* Force parse all lexed tokens when a single `\` is received */
		if (!ready && (line.size() == 1 && line[0] == TokenType::BACKSLASH)) {
			ready = true;
			methan0l.load();
		}

		if (ready)
			methan0l.run();
	} while (!methan0l.force_quit());
}

} /* namespace mtl */
