#include "test.h"

#include <iostream>

/* Module's entry point */
LOAD_MODULE
{
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
	function("retain_first_word", retain_first_word);
	
	/* Set local module variables */
	VAR(num) = 12345;
}

int my_sum(int a, int b)
{
	return a + b;
}

void retain_first_word(std::string &str)
{
	auto sep_idx = str.find(' ');
	if (sep_idx != std::string::npos)
		str.erase(sep_idx);
}

/* Another way of hooking functions to the interpreter
 * (functions used inside of this macro must be already declared somewhere) */
FUNCTION(foo)
void foo()
{
	std::cout << "Function foo()" << std::endl;
}

FUNCTION(list_vars)
void list_vars(mtl::Unit &unit)
{
	std::cout << "List of all variables:" << std::endl;
	for (auto &&var : unit.local())
		std::cout << "* " << var->first << " (" << var->second.type_name() << ")" << std::endl;
}