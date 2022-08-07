#include "test.h"

#include <iostream>

/*
 * Functions can be bound to a methan0l module using `function(name, callable)` or `FUNCTION(name)` macro
 * Module-local variables can be bound using VAR(name) macro
 *
 * Modules can then be loaded and used in multiple ways:
 *
 * (1) Loading as a box unit:
 * 		module = load_module("modules/test") 	(( Only the path to the module's directory is required if its .so file has the same name ))
 *		<% module.my_sum(11, 42)
 *
 * (2) Alternatively, loaded box unit can be imported into the current scope:
 *		module.import()
 *		test()
 *
 * (3) Or the module itself can be directly imported into the current scope by path:
 * 		using_module: "$:/modules/test"  (( Assuming the `modules` directory is in the same directory as the interpreter binary ))
 *		test()
 */

/* Module's entry point (optional) */
LOAD_MODULE
{
	std::cout << "Loading example module..." << std::endl;

	/* Register function via an InbuiltFunc lambda */
	function("test", [&](mtl::Args args) {
		return std::string("I'm alive!");
	});
	
	/* Register function via the function pointer */
	function("my_sum", my_sum);
	function("retain_first_word", retain_first_word);
	
	/* Set local module variables */
	VAR(num) = 12345;
	
	/* Bind a native class */
	native_class<TestClass>();

	std::cout << "Finished loading example module:" << std::endl;
	dump_data_table(*local_scope);
}

/* Initialize the binding process for `TestClass` */
NATIVE_CLASS_INIT(TestClass, {
	auto &binder = get_binder();
	binder.bind_constructor<std::string&>();
	binder.bind_method("get_stuff", &THIS_CLASS::get_stuff);
})


int my_sum(int a, int b)
{
	return a + b;
}

std::string &retain_first_word(std::string &str)
{
	auto sep_idx = str.find(' ');
	if (sep_idx != std::string::npos)
		str.erase(sep_idx);
	return str;
}

/* Another way of hooking functions to the interpreter
 * (functions used inside of this macro must be already declared somewhere) */
FUNCTION(foo)
void foo()
{
	std::cout << "Function foo()" << std::endl;
}

void dump_data_table(mtl::DataTable &table)
{
	for (auto &&var : *table.map_ptr())
		std::cout << "  * " << var.first << " (" << var.second.type_name() << ")" << std::endl;
}

FUNCTION(list_vars)
void list_vars(mtl::Unit &unit)
{
	std::cout << "List of all variables of unit:" << std::endl;
	dump_data_table(unit.local());
}