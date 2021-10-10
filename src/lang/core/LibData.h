#ifndef SRC_LANG_LIBDATA_H_
#define SRC_LANG_LIBDATA_H_

#include <string_view>

#include "../Library.h"

namespace mtl
{

/* Common data structure operators & functions */
class LibData: public Library
{
	public:
		LibData(ExprEvaluator *eval) : Library(eval) {}
		void load() override;

	private:
		static constexpr std::string_view KEY_LIST = "keys", VAL_LIST = "values";
		void load_operators();
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBDATA_H_ */
