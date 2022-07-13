#ifndef SRC_UTIL_META_TYPE_TRAITS_H_
#define SRC_UTIL_META_TYPE_TRAITS_H_

namespace mtl
{

template<typename T>
struct is_callable
{
	private:
		typedef char (&yes)[1];
		typedef char (&no)[2];

		struct Fallback
		{
				void operator()();
		};

		struct Derived: T, Fallback {};

		template<typename U, U> struct Check;

		template<typename >
		static yes test(...);

		template<typename C>
		static no test(Check<void (Fallback::*)(), &C::operator()>*);

	public:
		static const bool value = sizeof(test<Derived>(0)) == sizeof(yes);
};

}

#endif /* SRC_UTIL_META_TYPE_TRAITS_H_ */
