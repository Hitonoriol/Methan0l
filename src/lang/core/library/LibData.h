#ifndef SRC_LANG_LIBDATA_H_
#define SRC_LANG_LIBDATA_H_

#include <iostream>
#include <string_view>
#include <numeric>

#include <lang/Library.h>
#include <lang/core/Data.h>

#include <expression/LiteralExpr.h>
#include <util/meta/type_traits.h>
#include <util/containers.h>
#include <util/hash.h>

namespace mtl
{

/* Common data structure operators & functions */
class LibData: public Library
{
	public:
		using Library::Library;
		void load() override;

	private:
		void import_reference(const IdentifierExpr&);
		void load_operators();
		bool instanceof(Value &lhs, ExprPtr rhs_expr);
		void assert_type(Value &lhs, ExprPtr rhs_expr);

		Value if_not_same(ExprPtr lhs, ExprPtr rhs, bool convert = true);
};

} /* namespace mtl */

#endif /* SRC_LANG_LIBDATA_H_ */
