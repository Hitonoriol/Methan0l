#ifndef SRC_STRUCTURE_OBJECT_NATIVECLASS_H_
#define SRC_STRUCTURE_OBJECT_NATIVECLASS_H_

#include <util/class_binder.h>

namespace mtl
{

/*
 * Helper interface for binding C++ classes to Methan0l.
 * All calls to binder must be done inside the initialize() method.
 */
template<class T>
class NativeClass
{
	protected:
		ClassBinder<T> class_binder;
		virtual void initialize() = 0;

	public:
		NativeClass(Interpreter &context) : class_binder(context) {}

		virtual ~NativeClass() = default;

		inline void operator()()
		{
			initialize();
		}

		inline ClassBinder<T>& get_binder()
		{
			return class_binder;
		}

		static inline T& native(Object &obj)
		{
			return ClassBinder<T>::as_native(obj);
		}
};

/* Declare a NativeClass<> binder, where:
 * 		`name` is both the name of the new C++ class and methan0l class
 * 		`...` is the native class name / template specification being bound to the interpreter. */
#define NATIVE_CLASS_DECL(name, ...) \
	class name: public mtl::NativeClass<JOIN(__VA_ARGS__)> \
	{ \
		public: \
			using bound_class = JOIN(__VA_ARGS__); \
			using mtl::NativeClass<JOIN(__VA_ARGS__)>::NativeClass; \
			void initialize() override; \

#define NATIVE_CLASS_DECL_END(...) };


#define NATIVE_CLASS(name, ...) \
		NATIVE_CLASS_DECL(name, __VA_ARGS__) \
		NATIVE_CLASS_DECL_END()

/*   For use only in source files.
 *   `name` is the name of the NativeClass<> binder declared through macros above.
 *   `...` is a scope statement ({...}) containing a constructor binding and all
 * native class method bindings you wish to expose to the interpreter. */
#define NATIVE_CLASS_BINDING(name, ...) \
	void name::initialize() \
	{ \
		class_binder.set_name(STR(name)); \
		class_binder.get_class().set_native_id<name>(); \
		JOIN(__VA_ARGS__) \
		class_binder.register_class(); \
		LOG("Bound a native class: " << mtl::type_name<bound_class>() << " [" << class_binder.get_class().get_name() << "]") \
	}

/* Macros for use inside the NATIVE_CLASS_BINDING(...): */

/* For convenient inline method definitions */
#define STANDARD_METHOD(name) class_binder.register_method(name) = []
#define METHOD(name) STANDARD_METHOD(STR(name))
#define CONSTRUCTOR STANDARD_METHOD(Methods::Constructor)

/*   For use inside of the METHOD(...) macro above as the first argument.
 * (treat this like an explicit `this` parameter) */
#define OBJ mtl::Object &this_obj

/*   For referring to `this` object from inside the inline-defined methods
 * (e.g. via ClassBinder<C>::register_method(std::string_view, F&&)) */
#define THIS OBJECT(class_binder, this_obj)

/*   For referring to `this` object from inside the proxy-methods (methods that
 * need to access the mtl::Object representation of the current object or the
 * interpreter context; such methods must be declared as static member or non-member
 * functions and have `OBJ` in place of the first argument).
 *   `class_name` - name of the binder class inheriting from NativeClass<C>. */
#define NATIVE(class_name) class_name::native(this_obj)

/* For referring to the interpreter context current method is called from */
#define CONTEXT this_obj.context()

/* For convenient referring to the bound native class */
#define THIS_CLASS CLASS(class_binder)

/* Convenience macros for `bind_method` and `bind_constructor` methods of ClassBinder<C> */
#define BIND_CONSTRUCTOR(...) class_binder.bind_constructor<JOIN(__VA_ARGS__)>();
#define BIND_METHOD(name) class_binder.bind_method(#name, &THIS_CLASS::name);

/*   Bind a static member / non-member function as a mtl::Class method.
 * Requires function's full name including class it's a member of (if any).
 * Requires first argument of the function to be a `mtl::Object&` (`OBJ` macro can be used). */
#define BIND_PROXY_METHOD(name) class_binder.register_method(#name, &name);

/* ----------------------------------------------------- */

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_NATIVECLASS_H_ */
