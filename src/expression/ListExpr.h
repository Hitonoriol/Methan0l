#ifndef SRC_EXPRESSION_LISTEXPR_H_
#define SRC_EXPRESSION_LISTEXPR_H_

#include "Expression.h"

namespace mtl
{

class ListExpr: public Expression
{
	private:
		ExprList exprs;

	public:
		ListExpr(ExprList exprs) : exprs(exprs)
		{
		}

		ExprList& raw_list()
		{
			return exprs;
		}

		Value evaluate(ExprEvaluator &evaluator) override
		{
			ValList list;

			for (auto expr : exprs)
				list.push_back(expr->evaluate(evaluator));

			return Value(list);
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{List Expression // " << exprs.size() << " elements}";
		}
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_LISTEXPR_H_ */
