#ifndef EXPRESSION_IDENTIFIEREXPR_H_
#define EXPRESSION_IDENTIFIEREXPR_H_

#include "Expression.h"
#include "LiteralExpr.h"
#include "../structure/Unit.h"
#include "../util/util.h"

namespace mtl
{

class IdentifierExpr: public Expression
{
	protected:
		std::string name;
		bool global = false;

	public:
		IdentifierExpr() = default;

		IdentifierExpr(std::string name, bool global) : name(name), global(global)
		{
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			Value reserved = eval_reserved(name);
			if (!reserved.empty())
				return reserved;

			return referenced_value(eval);
		}

		virtual Value& referenced_value(ExprEvaluator &eval)
		{
			return eval.get(name, global);
		}

		virtual void assign(ExprEvaluator &eval, Value val)
		{
			create_if_nil(eval);
			referenced_value(eval) = val;
		}

		virtual void create_if_nil(ExprEvaluator &eval)
		{
			DataTable *scope = eval.scope_lookup(name, global);
			if (!scope->exists(name))
				scope->set(name, Value());
		}

		void execute(mtl::ExprEvaluator &evaluator) override
		{
			Value val = evaluate(evaluator);
			LiteralExpr::exec_literal(evaluator, val);
		}

		bool is_global()
		{
			return global;
		}

		std::string& get_name()
		{
			return name;
		}

		static Value eval_reserved(std::string &name)
		{
			if (name == Token::reserved(Word::NEW_LINE))
				return NEW_LINE;

			if (name == Token::reserved(Word::NIL))
				return NIL;

			return NO_VALUE;
		}

		inline static std::string get_name(ExprPtr expr)
		{
			return try_cast<IdentifierExpr>(expr).name;
		}

		std::ostream& info(std::ostream &str) override
		{
			return str << "{Identifier Expression name = " << name << "}";
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_IDENTIFIEREXPR_H_ */
