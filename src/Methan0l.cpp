#include <deque>
#include <fstream>
#include <iostream>
#include <string>

#include "expression/Expression.h"
#include "Interpreter.h"
#include "methan0l_type.h"
#include "structure/Unit.h"
#include "structure/Value.h"
#include "expression/IdentifierExpr.h"
#include "util.h"

using namespace std;

void run_file(mtl::Interpreter &methan0l, char *filename)
{
	string src_name = string(filename);
	methan0l.load(methan0l.load_file(src_name));
	mtl::Value ret = methan0l.run();

	if (!ret.empty())
		cout << "Main returned: " << ret << std::endl;
}

std::unordered_map<std::string, std::function<void(mtl::Interpreter&)>> intr_cmds {
		{ "info",
				[](auto &mtl)
					{
						mtl.print_info();
					}
		}
};

bool process_commands(mtl::Interpreter &methan0l)
{
	mtl::ExprPtr cmdexpr = methan0l.program().expressions().front();
	if (!instanceof<mtl::IdentifierExpr>(cmdexpr.get()))
		return false;

	std::string cmd = mtl::IdentifierExpr::get_name(cmdexpr);
	auto f_cmd = intr_cmds.find(cmd);
	if (f_cmd != intr_cmds.end()) {
		f_cmd->second(methan0l);
		return true;
	}

	return false;
}

void interactive_mode(mtl::Interpreter &methan0l)
{
	methan0l.preserve_data(true);
	string expr;
	do {
		cout << "[Methan0l] <-- ";
		getline(std::cin, expr);

		if (!expr.empty()) {
			methan0l.load(expr);
			if (process_commands(methan0l))
				continue;
		}

		methan0l.run();
	} while (!methan0l.force_quit());
}

int main(int argc, char **argv)
{
	mtl::Interpreter methan0l;

	if (argc > 1)
		run_file(methan0l, argv[1]);
	else
		interactive_mode(methan0l);

	return 0;
}
