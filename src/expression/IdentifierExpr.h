#ifndef EXPRESSION_IDENTIFIEREXPR_H_
#define EXPRESSION_IDENTIFIEREXPR_H_

#include "Expression.h"
#include "LiteralExpr.h"
#include "structure/Unit.h"
#include "util/util.h"
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

		Value evaluate(Interpreter &eval) override;

		virtual Value& referenced_value(Interpreter &eval, bool follow_refs = true);
		virtual Value& assign(Interpreter &eval, Value val);
		virtual void create_if_nil(Interpreter &eval);

		void execute(Interpreter &evaluator) override;

		bool is_global() const;
		const std::string& get_name() const;

		static Value eval_reserved(const std::string &name);

		inline static const std::string& get_name(Expression *expr)
		{
			return try_cast<IdentifierExpr>(expr).name;
		}

		inline static const std::string& get_name(ExprPtr expr)
		{
			return get_name(expr.get());
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* EXPRESSION_IDENTIFIEREXPR_H_ */
