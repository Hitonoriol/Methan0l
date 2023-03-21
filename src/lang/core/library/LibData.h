#ifndef SRC_LANG_LIBDATA_H_
#define SRC_LANG_LIBDATA_H_

#include <iostream>
#include <string_view>
#include <numeric>

#include "lang/Library.h"
#include "lang/core/Data.h"

#include "expression/LiteralExpr.h"
#include "util/meta/type_traits.h"
#include "util/containers.h"
#include "util/hash.h"

namespace mtl
{

/* Common data structure operators & functions */
class LibData: public Library
{
	public:
		using Library::Library;
		void load() override;

	private:
		static constexpr std::string_view KEY_LIST = "keys", VAL_LIST = "values";
		static DblBinOperation summator, multiplicator;

		void import_reference(const IdentifierExpr&);
		void load_operators();
		void load_container_funcs();
		bool instanceof(Value &lhs, Value &rhs);
		void assert_type(Value &lhs, Value &rhs);

		std::pair<size_t, double> accumulate(Value &callable, Args &args, double init, DblBinOperation operation);
		std::pair<size_t, double> dispatch_accumulate(Args &args, double init, DblBinOperation operation);

		double mean(Args&);

		Value if_not_same(ExprPtr lhs, ExprPtr rhs, bool convert = true);
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBDATA_H_ */
