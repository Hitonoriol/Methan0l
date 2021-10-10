#ifndef SRC_LANG_LIBSTRING_H_
#define SRC_LANG_LIBSTRING_H_

#include "../Library.h"

namespace mtl
{

class LibString: public Library
{
	public:
		LibString(ExprEvaluator *eval) : Library(eval) {}
		void load() override;

	private:
		void load_operators();
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBSTRING_H_ */
