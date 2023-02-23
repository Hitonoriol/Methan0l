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
		TypeID lhs_val_type = Type::NIL;
		bool remove;
		bool insert;

		inline bool append()
		{
			return idx == nullptr;
		}

		void clear_container(Value &contval);

		Value& indexed_element(Interpreter &context);
		Value& indexed_element(Interpreter &context, ValList &list);
		Value& indexed_element(Interpreter &context, ValMap &map);
		Value& indexed_element(Interpreter &context, ValSet &set);
		Value& indexed_element(Interpreter &context, DataTable &table);

	public:
		IndexExpr(ExprPtr list_idfr, ExprPtr idx, bool remove, bool insert) :
				lhs(list_idfr), idx(idx), remove(remove), insert(insert)
		{
		}

		Value evaluate(Interpreter &context) override;
		void execute(mtl::Interpreter &context) override;

		Value& assign(Interpreter &context, Value val) override;
		Value& referenced_value(Interpreter &context, bool follow_refs = true) override;

		ExprPtr get_lhs();
		ExprPtr get_rhs();

		void create_if_nil(Interpreter &context) override
		{
			return;
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_INDEXEXPR_H_ */
