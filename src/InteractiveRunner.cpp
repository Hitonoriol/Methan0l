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

const InteractiveRunner::CommandMap InteractiveRunner::intr_cmds
{
		{ "version",
				[](auto &mtl)
					{
						std::cout << FULL_VERSION_STR << std::endl;
					}
		},
		{ "info",
				[](auto &mtl)
					{
						mtl.print_info();
					}
		},
		{ "size_info",
				[](auto &mtl)
					{
						mtl.size_info();
					}
		},
		{ "expressions",
				[](auto &mtl)
					{
						for (auto &&expr : mtl.program().expressions())
							std::cout << "\t" << expr->info() << std::endl;
					}
		}
};

InteractiveRunner::InteractiveRunner(Interpreter &methan0l) : methan0l(methan0l)
{
}

/* Format: !!command */
bool InteractiveRunner::process_commands(const std::string &cmd)
{
	if (methan0l.force_quit())
		return false;

	if (cmd.size() < 3
			|| !(cmd[0] == TokenType::EXCLAMATION && cmd[1] == TokenType::EXCLAMATION))
		return false;

	auto f_cmd = intr_cmds.find(std::string_view(cmd).substr(2));
	if (f_cmd != intr_cmds.end()) {
		f_cmd->second(methan0l);
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
	std::cout << FULL_VERSION_STR << " on " << get_os() << std::endl;
	methan0l.preserve_data(true);
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
		/* Empty line when multi-line parsing is in progress */
		else if (!ready) {
			ready = true;
			methan0l.load();
		}

		if (ready)
			methan0l.run();
	} while (!methan0l.force_quit());
}

} /* namespace mtl */
