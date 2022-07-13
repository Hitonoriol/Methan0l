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
	private:
		ClassBinder<T> class_binder;

	protected:
		virtual void initialize() = 0;

	public:
		NativeClass(ExprEvaluator &eval) : class_binder(eval) {}

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

#define NATIVE_CLASS(name, ...) \
	class name: public NativeClass<JOIN(__VA_ARGS__)> \
	{ \
		public: \
			using bound_class = JOIN(__VA_ARGS__); \
			using NativeClass<JOIN(__VA_ARGS__)>::NativeClass; \
			void initialize() override; \
	};


} /* namespace mtl */

#endif /* SRC_STRUCTURE_OBJECT_NATIVECLASS_H_ */
