#ifndef EXPRESSION_IDENTIFIEREXPR_H_
#define EXPRESSION_IDENTIFIEREXPR_H_

#include <parser/expression/Expression.h>

#include <structure/Unit.h>
#include <util/util.h>
#include <expression/LiteralExpr.h>
#include <expression/BinaryOperatorExpr.h>

namespace mtl
{

class IdentifierExpr: public Expression
{
	protected:
		std::string name;
		bool global = false;

	public:
		IdentifierExpr() = default;
		IdentifierExpr(const std::string &name, bool global) : name(name), global(global) {}

		virtual Value& referenced_value(Interpreter &context, bool follow_refs = true);
		virtual Value& assign(Interpreter &context, Value val);
		virtual void create_if_nil(Interpreter &context);

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

		inline static const std::string& get_name_or_default(Expression *expr)
		{
			if (!instanceof<IdentifierExpr>(expr))
				return mtl::empty_string;

			return try_cast<IdentifierExpr>(expr).name;
		}

		inline static const std::string& get_name_or_default(ExprPtr expr)
		{
			return get_name_or_default(expr.get());
		}

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* EXPRESSION_IDENTIFIEREXPR_H_ */
