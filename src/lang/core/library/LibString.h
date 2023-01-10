#ifndef SRC_LANG_LIBSTRING_H_
#define SRC_LANG_LIBSTRING_H_

#include <regex>

#include "lang/Library.h"

namespace mtl
{

class LibString: public Library
{
	public:
		LibString(Interpreter *eval) : Library(eval) {}
		void load() override;

		static void format(std::string &fmt, const std::vector<std::string> &sargs);

	private:

		void load_operators();
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBSTRING_H_ */
