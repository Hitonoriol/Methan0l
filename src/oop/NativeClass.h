#ifndef SRC_STRUCTURE_OBJECT_NATIVECLASS_H_
#define SRC_STRUCTURE_OBJECT_NATIVECLASS_H_

#include <util/class_binder.h>
#include <util/string.h>

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

/* Update methan0l class' name and native id */
#define SET_CLASS_ID(name) \
	class_binder.set_name(mtl::strip_name_scope(STR(name))); \
	class_binder.get_class().set_id<bound_class>();

/*   For use only in source files.
 *   `name` is the name of the NativeClass<> binder declared through macros above.
 *   `...` is a scope statement ({...}) containing a constructor binding and all
 * native class method bindings you wish to expose to the interpreter. */
#define CLASS_BINDING(name, ...) \
		{ \
			SET_CLASS_ID(name) \
			STANDARD_METHOD(Methods::Copy) (OBJ) { \
				return mtl::Object::copy_native<THIS_CLASS>(this_obj); \
			}; \
			JOIN(__VA_ARGS__) \
			class_binder.register_class(); \
			LOG("Bound a native class: " << mtl::type_name<bound_class>() << " [" << class_binder.get_class().get_name() << "]") \
		}

#define NATIVE_CLASS_BINDING(name, ...) \
	void name::initialize() \
	CLASS_BINDING(name, JOIN(__VA_ARGS__))

/* Methan0l runtime (not backed by any native class) class declaration & definition */
#define MTL_CLASS_NAME(name) MTL##name##Class
#define METHAN0L_CLASS(name) NATIVE_CLASS(MTL_CLASS_NAME(name), MTL_CLASS_NAME(name))
#define METHAN0L_CLASS_BODY(name, ...) \
		void MTL_CLASS_NAME(name)::initialize() \
		CLASS_BINDING(MTL_CLASS_NAME(name), {SET_CLASS_ID(name);} JOIN(__VA_ARGS__))

#define REGISTER_METHAN0L_CLASS(context, name) (context).register_class<MTL_CLASS_NAME(name)>();
#define REGISTER_CLASS_BINDING(context, name) (context).register_class<name>();

/* Mark classes as interfaces or methods as abstract */
#define INTERFACE CONSTRUCTOR (OBJ) UNIMPLEMENTED;
#define ABSTRACT_METHOD(name) METHOD(name) (OBJ) UNIMPLEMENTED;
#define NON_CONSTRUCTIBLE INTERFACE
#define UNIMPLEMENTED_METHOD(name) ABSTRACT_METHOD(name)

/* Allows to specify base classes of the bound class */
#define INHERITS(name) \
	{ \
		class_binder.get_class() \
			.inherit(STR(name)); \
	}

#define IMPLEMENTS(name) \
	{ \
		class_binder.get_class() \
			.implement(STR(name)); \
	} \

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
#define CONSTRUCTOR_DARGS(...) class_binder.set_constructor_default_args(JOIN(__VA_ARGS__));

#define BIND_METHOD_AS(bind_as, method_name) class_binder.bind_method(bind_as, &THIS_CLASS::method_name);
#define BIND_METHOD(name) BIND_METHOD_AS(#name, name)

/* Bind a method with specified default arguments */
#define BIND_DARGS_METHOD_AS(bind_as, method_name, ...) \
	class_binder.bind_method(bind_as, &THIS_CLASS::method_name, JOIN(__VA_ARGS__));
#define BIND_DARGS_METHOD(name, ...) BIND_DARGS_METHOD_AS(#name, name, JOIN(__VA_ARGS__))

/*   Bind a static member (PROXY) / non-member (EXTERNAL) function as a mtl::Class method.
 * Requires function's full name including class it's a member of (if any).
 * Requires first argument of the function to be a `mtl::Object&` (`OBJ` macro can be used). */
#define BIND_EXTERNAL_METHOD_AS(bind_as, method_name) class_binder.register_method(bind_as, &method_name);
#define BIND_PROXY_METHOD(name) BIND_EXTERNAL_METHOD_AS(#name, THIS_CLASS::name)
#define BIND_EXTERNAL_METHOD(name) BIND_EXTERNAL_METHOD_AS(#name, name)
#define BIND_EXTERNAL_DARGS_METHOD_AS(bind_as, method_name, ...) \
	class_binder.register_method(bind_as, method_name, JOIN(__VA_ARGS__));
#define BIND_EXTERNAL_DARGS_METHOD(name, ...) \
		BIND_EXTERNAL_DARGS_METHOD_AS(#name, name, JOIN(__VA_ARGS__))

/*   Bind a "mutator" method (a method that's intended to return `this`).
 * The native method being bound isn't required to return anything. */
#define BIND_MUTATOR_METHOD_AS(bind_as, method_name) \
	class_binder.bind_mutator_method(bind_as, &THIS_CLASS::method_name);
#define BIND_MUTATOR_METHOD(name) BIND_MUTATOR_METHOD_AS(STR(name), name)
#define BIND_EXTERNAL_MUTATOR_METHOD_AS(bind_as, method_name) \
	class_binder.bind_external_mutator_method(bind_as, &method_name);
#define BIND_EXTERNAL_MUTATOR_METHOD(name) BIND_EXTERNAL_MUTATOR_METHOD_AS(STR(name), name)

/* ----------------------------------------------------- */

#define METHOD_ALIAS(method_name, alias) \
	STANDARD_METHOD(alias) (Args args) { \
		return Object::get_this(args).invoke_method(STR(method_name), args); \
	}; \

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_NATIVECLASS_H_ */
