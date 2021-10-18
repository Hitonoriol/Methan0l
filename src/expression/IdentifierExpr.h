#ifndef EXPRESSION_IDENTIFIEREXPR_H_
#define EXPRESSION_IDENTIFIEREXPR_H_

#include "Expression.h"
#include "LiteralExpr.h"
#include "../structure/Unit.h"
#include "../util/util.h"
#include "BinaryOperatorExpr.h"

namespace mtl
{

class IdentifierExpr: public Expression
{
	protected:
		std::string name;
		bool global = false;

	public:
		IdentifierExpr() = default;
		IdentifierExpr(std::string name, bool global) : name(name), global(global) {}

		Value evaluate(ExprEvaluator &eval) override;

		virtual Value& referenced_value(ExprEvaluator &eval);
		virtual void assign(ExprEvaluator &eval, Value val);
		virtual void create_if_nil(ExprEvaluator &eval);

		void execute(mtl::ExprEvaluator &evaluator) override;

		bool is_global();
		std::string& get_name();

		static Value eval_reserved(std::string &name);
		static std::string get_name(ExprPtr expr);

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* EXPRESSION_IDENTIFIEREXPR_H_ */
