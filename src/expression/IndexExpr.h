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
		bool insert;

		inline bool append()
		{
			return idx == nullptr;
		}

		void clear_container(Value &contval);

		Value& indexed_element(ExprEvaluator &evaluator);
		Value& indexed_element(ExprEvaluator &evaluator, ValList &list);
		Value& indexed_element(ExprEvaluator &evaluator, ValMap &map);
		Value& indexed_element(ExprEvaluator &evaluator, ValSet &set);
		Value& indexed_element(ExprEvaluator &evaluator, DataTable &table);

	public:
		TRANSLATABLE
		IndexExpr(ExprPtr list_idfr, ExprPtr idx, bool remove, bool insert) :
				lhs(list_idfr), idx(idx), remove(remove), insert(insert)
		{
		}

		Value evaluate(ExprEvaluator &evaluator) override;
		void execute(mtl::ExprEvaluator &evaluator) override;

		Value& assign(ExprEvaluator &eval, Value val) override;
		Value& referenced_value(ExprEvaluator &eval, bool follow_refs = true) override;

		ExprPtr get_lhs();
		ExprPtr get_rhs();

		void create_if_nil(ExprEvaluator &eval) override
		{
			return;
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_INDEXEXPR_H_ */
