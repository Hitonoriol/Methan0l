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
		LiteralExpr(Type val_type, Token token)
		{
			std::string &tokstr = token.get_value();
			auto type = token.get_type();

			if (val_type == Type::INTEGER)
				value = std::stol(tokstr);

			else if (val_type == Type::DOUBLE)
				value = std::stod(tokstr);

			else if (type == TokenType::BOOLEAN)
				value = tokstr == Token::reserved(Word::TRUE);

			else if (type == TokenType::STRING)
				value = strip_quotes(tokstr);

			else if (val_type == Type::CHAR)
				value = strip_quotes(tokstr)[0];
		}

		template<typename T>
		LiteralExpr(T value) : value(value)
		{
		}

		LiteralExpr(const Value &val) : value(val)
		{
		}

		LiteralExpr() : value(NoValue())
		{
		}

		bool is_empty()
		{
			return value.empty();
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			return Value(value);
		}

		Value raw_value()
		{
			return value;
		}

		Value& raw_ref()
		{
			return value;
		}

		void execute(ExprEvaluator &evaluator) override
		{
			Value evald = evaluate(evaluator);
			exec_literal(evaluator, evald);
		}

		static std::shared_ptr<LiteralExpr> empty()
		{
			return std::make_shared<LiteralExpr>();
		}

		template<typename T>
		static std::shared_ptr<LiteralExpr> create(T val)
		{
			return std::make_shared<LiteralExpr>(val);
		}

		static void exec_literal(ExprEvaluator &evaluator, Value &val)
		{
			if (val.empty())
				return;

			switch (val.type()) {
			case Type::UNIT: {
				Unit unit = val.get<Unit>();
				unit.call();
				evaluator.execute(unit);
				break;
			}

			default:
				std::cout << val << std::endl;
				break;
			}
		}

		std::ostream& info(std::ostream &str) override
		{
			return Expression::info(str << "{Literal Expression: `" << value << "`}");
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_LITERALEXPR_H_ */
