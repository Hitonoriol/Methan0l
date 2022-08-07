#ifndef MODULES_TEST_TEST_H_
#define MODULES_TEST_TEST_H_

#include <methan0l.h>

int my_sum(int a, int b);
std::string &retain_first_word(std::string&);
void foo();
void list_vars(mtl::Unit&);
void dump_data_table(mtl::DataTable&);

/* Class binding example
 *
 * Native classes can be bound to the interpreter either by using:
 * (1) short-form binding:
 * 		In header: NATIVE_CLASS(bound_name, native_class_name)
 *		In source file: 
 *			NATIVE_CLASS_INIT(bound_name, {
 *				... // Use get_binder() to get this class's binder instance and bind its constructor and methods using it.
 *			})
 *			
 * In this case `bound_name` and `native_class_name` must be different
 *  as this implies that class `native_class_name` is already declared.
 *
 * (2) or expendable declaration form
 *  (not really recommended as it inherits NativeClass<> and already has memory overhead of one std::shared_ptr and one reference):
 * 		In header:
 *			NATIVE_CLASS_DECL(bound_name, native_class_name)
 *				... // Declare fields & methods as usual
 *			NATIVE_CLASS_DECL_END()
 *		In source file: identical to (1) + define declared methods as usual: `<type> bound_name::method_name(...) {...}`
 *
 * Note that classes are registered via the interpreter's type manager globally and are not bound to the module.
 */

namespace native
{

class TestClass
{
	private:
		std::string stuff;
	
	public:
		inline TestClass(const std::string &str)
			: stuff(str) {}

		inline const std::string &get_stuff()
		{
			return stuff;
		}
};

}

/* You can define a class in a separate namespace to use the same name for bound class */
NATIVE_CLASS(TestClass, native::TestClass)

#endif /* MODULES_TEST_TEST_H_ */