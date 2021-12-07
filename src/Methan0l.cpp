#include "Interpreter.h"
#include "Runner.h"
#include "InteractiveRunner.h"

int main(int argc, char **argv)
{
	mtl::Interpreter methan0l;

	if (argc > 1)
		return mtl::Runner::run_file(methan0l, argc, argv);

	mtl::InteractiveRunner().start(methan0l);
	return 0;
}
