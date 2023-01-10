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

		Value& indexed_element(Interpreter &evaluator);
		Value& indexed_element(Interpreter &evaluator, ValList &list);
		Value& indexed_element(Interpreter &evaluator, ValMap &map);
		Value& indexed_element(Interpreter &evaluator, ValSet &set);
		Value& indexed_element(Interpreter &evaluator, DataTable &table);

	public:
		IndexExpr(ExprPtr list_idfr, ExprPtr idx, bool remove, bool insert) :
				lhs(list_idfr), idx(idx), remove(remove), insert(insert)
		{
		}

		Value evaluate(Interpreter &evaluator) override;
		void execute(mtl::Interpreter &evaluator) override;

		Value& assign(Interpreter &eval, Value val) override;
		Value& referenced_value(Interpreter &eval, bool follow_refs = true) override;

		ExprPtr get_lhs();
		ExprPtr get_rhs();

		void create_if_nil(Interpreter &eval) override
		{
			return;
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_INDEXEXPR_H_ */
