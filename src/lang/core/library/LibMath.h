#ifndef SRC_LANG_LIBMATH_H_
#define SRC_LANG_LIBMATH_H_

#include <lang/Library.h>

namespace mtl
{

/* Bindings to cmath */
class LibMath: public Library
{
	private:
		static constexpr double PI = 3.14159265358979323846264338327950288,
				DEGREE = PI / 180.0,
				RADIAN = 180.0 / PI;

	public:
		using Library::Library;
		void load() override;
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBMATH_H_ */
