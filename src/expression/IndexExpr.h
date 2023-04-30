#ifndef SRC_EXPRESSION_INDEXEXPR_H_
#define SRC_EXPRESSION_INDEXEXPR_H_

#include "IdentifierExpr.h"

namespace mtl
{

class Unit;

class IndexExpr: public IdentifierExpr
{
	private:
		ExprPtr lhs, idx;
		bool remove;
		bool insert;

		Value& indexed_element(Interpreter&, DataTable&);

	public:
		IndexExpr(ExprPtr list_idfr, ExprPtr idx, bool remove, bool insert) :
				lhs(list_idfr), idx(idx), remove(remove), insert(insert)
		{}

		Value evaluate(Interpreter &context) override;
		void execute(mtl::Interpreter &context) override;

		Value& assign(Interpreter &context, Value val) override;
		Value& referenced_value(Interpreter &context, bool follow_refs = true) override;

		ExprPtr get_lhs();
		ExprPtr get_rhs();

		void create_if_nil(Interpreter&) override UNIMPLEMENTED

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* SRC_EXPRESSION_INDEXEXPR_H_ */
