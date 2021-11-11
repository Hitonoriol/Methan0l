#include "InteractiveRunner.h"

#include <deque>
#include <memory>
#include <utility>

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
		}
};

bool InteractiveRunner::process_commands(Interpreter &methan0l)
{
	auto &exprs = methan0l.program().expressions();
	if (methan0l.force_quit() || exprs.empty())
		return false;

	ExprPtr cmdexpr = exprs.front();
	if (!instanceof<IdentifierExpr>(cmdexpr.get()))
		return false;

	std::string cmd = IdentifierExpr::get_name(cmdexpr);
	auto f_cmd = intr_cmds.find(cmd);
	if (f_cmd != intr_cmds.end()) {
		f_cmd->second(methan0l);
		return true;
	}

	return false;
}

void InteractiveRunner::start(Interpreter &methan0l)
{
	std::cout << FULL_VERSION_STR << " on " << get_os() << std::endl;
	methan0l.preserve_data(true);
	std::string expr;
	do {
		std::cout << "[Methan0l] <-- " << std::flush;
		getline(std::cin, expr);
		if (!expr.empty()) {
			methan0l.load(expr);
			if (process_commands(methan0l))
				continue;
		}
		methan0l.run();
	} while (!methan0l.force_quit());
}

} /* namespace mtl */
