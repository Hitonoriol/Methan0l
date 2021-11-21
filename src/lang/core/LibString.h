#ifndef SRC_LANG_LIBSTRING_H_
#define SRC_LANG_LIBSTRING_H_

#include <regex>

#include "lang/Library.h"

namespace mtl
{

class LibString: public Library
{
	public:
		LibString(ExprEvaluator *eval) : Library(eval) {}
		void load() override;

	private:
		static std::regex string_fmt;

		void load_operators();
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBSTRING_H_ */
