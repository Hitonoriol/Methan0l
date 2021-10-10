#ifndef SRC_EXPRESSION_INDEXEXPR_H_
#define SRC_EXPRESSION_INDEXEXPR_H_

#include "Expression.h"
#include "LiteralExpr.h"
#include "IdentifierExpr.h"

namespace mtl
{

class Unit;

class IndexExpr: public IdentifierExpr
{
	private:
		ExprPtr lhs, idx;
		Type lhs_val_type = Type::NIL;
		bool remove;

		inline bool append()
		{
			return idx == nullptr;
		}

		Value& indexed_element(ExprEvaluator &evaluator);
		Value& indexed_element(ExprEvaluator &evaluator, ValList &list);
		Value& indexed_element(ExprEvaluator &evaluator, ValMap &map);

	public:
		IndexExpr(ExprPtr list_idfr, ExprPtr idx, bool remove) :
				lhs(list_idfr), idx(idx), remove(remove)
		{
		}

		Value evaluate(ExprEvaluator &evaluator) override;

		void assign(ExprEvaluator &eval, Value val) override;
		Value& referenced_value(ExprEvaluator &eval) override;

		void create_if_nil(ExprEvaluator &eval) override
		{
			return;
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_INDEXEXPR_H_ */
