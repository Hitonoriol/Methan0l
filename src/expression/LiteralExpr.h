#ifndef EXPRESSION_LITERALEXPR_H_
#define EXPRESSION_LITERALEXPR_H_

#include "Expression.h"
#include "../util/util.h"
#include "../structure/Value.h"

namespace mtl
{

struct Value;

class LiteralExpr: public Expression
{
	private:
		Value value;

	public:
		LiteralExpr(Type val_type, const Token&);

		template<typename T>
		LiteralExpr(T value) : value(value)
		{
		}

		LiteralExpr(const Value &val);
		LiteralExpr();

		bool is_empty();

		Value evaluate(ExprEvaluator &eval);

		Value raw_value();
		Value& raw_ref();

		void execute(ExprEvaluator &evaluator) override;

		static std::shared_ptr<LiteralExpr> empty();
		template<typename T>
		static std::shared_ptr<LiteralExpr> create(T val)
		{
			return std::make_shared<LiteralExpr>(val);
		}

		static void exec_literal(ExprEvaluator &evaluator, Value &val);

		std::ostream& info(std::ostream &str) override;
};

} /* namespace mtl */

#endif /* EXPRESSION_LITERALEXPR_H_ */
