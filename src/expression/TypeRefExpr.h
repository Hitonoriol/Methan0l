#ifndef SRC_EXPRESSION_TYPEREFEXPR_H_
#define SRC_EXPRESSION_TYPEREFEXPR_H_

#include "IdentifierExpr.h"

namespace mtl
{

class TypeRefExpr : public IdentifierExpr
{
	public:
		TypeRefExpr(const std::string &type_name);
		virtual ~TypeRefExpr() = default;

		Value& referenced_value(Interpreter &context, bool follow_refs = true) override UNIMPLEMENTED
		Value& assign(Interpreter &context, Value val) override UNIMPLEMENTED
		void create_if_nil(Interpreter &context) override UNIMPLEMENTED
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_TYPEREFEXPR_H_ */
