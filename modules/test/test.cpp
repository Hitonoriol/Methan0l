#include "test.h"

/* Module's entry point */
extern "C" void load(mtl::ExprEvaluator *evaluator)
{
	init_module(evaluator);

	/*
	 * Functions defined using `function(name, ...)` overloads will be accessible
	 * inside of this module's scope, for example:
	 * 		module = load_module("modules/test/test") 	(( .so extension is optional ))
	 *		<% module.my_sum(11, 42)
	 * Alternatively, they can be imported into the current scope, as with any other Unit:
	 *		module.import()
	 *		test()
	 */

	/* Register function via an InbuiltFunc lambda */
	function("test", [&](mtl::Args args) {
		return std::string("I'm alive!");
	});
	
	/* Register function via the function pointer */
	function("my_sum", my_sum);
}

int my_sum(int a, int b)
{
	return a + b;
}