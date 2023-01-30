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
};

#define NATIVE_CLASS_DECL(name, ...) \
	class name: public mtl::NativeClass<JOIN(__VA_ARGS__)> \
	{ \
		public: \
			using bound_class = JOIN(__VA_ARGS__); \
			using mtl::NativeClass<JOIN(__VA_ARGS__)>::NativeClass; \
			void initialize() override; \

#define NATIVE_CLASS_DECL_END(...) };

/* Declare a NativeClass<> binder, where:
 * 		`name` is both the name of the new C++ class and methan0l class
 * 		`...` is the native class name / template specification being bound to the interpreter. */
#define NATIVE_CLASS(name, ...) \
		NATIVE_CLASS_DECL(name, __VA_ARGS__) \
		NATIVE_CLASS_DECL_END()

#define NATIVE_CLASS_INIT(name, ...) \
	void name::initialize() \
	{ \
		class_binder.set_name(STR(name)); \
		class_binder.get_class().set_native_id<name>(); \
		JOIN(__VA_ARGS__) \
		class_binder.register_class(); \
		LOG("Bound a native class: " << mtl::type_name<bound_class>() << " [" << class_binder.get_class().get_name() << "]") \
	}

#define OBJ mtl::Object &this_obj
#define METHOD(...) [](JOIN(__VA_ARGS__))
#define THIS OBJECT(class_binder, this_obj)
#define THIS_CLASS CLASS(class_binder)

#define BIND_CONSTRUCTOR(...) binder.bind_constructor<JOIN(__VA_ARGS__)>();
#define BIND_METHOD(name) binder.bind_method(#name, &THIS_CLASS::name);

} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_NATIVECLASS_H_ */
