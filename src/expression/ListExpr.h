#ifndef SRC_EXPRESSION_LISTEXPR_H_
#define SRC_EXPRESSION_LISTEXPR_H_

#include <parser/expression/Expression.h>

#include <type.h>

namespace mtl
{

class ListExpr: public Expression
{
	private:
		ExprList exprs;
		bool as_set;

	public:
		ListExpr(ExprList exprs, bool as_set = false);
		
		ExprList& raw_list();
		bool is_set();

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_LISTEXPR_H_ */
