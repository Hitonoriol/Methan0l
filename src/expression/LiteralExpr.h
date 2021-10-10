#ifndef EXPRESSION_LITERALEXPR_H_
#define EXPRESSION_LITERALEXPR_H_

#include "Expression.h"
#include "../util/util.h"

namespace mtl
{

class LiteralExpr: public Expression
{
	private:
		ValueContainer value;

	public:
		LiteralExpr(Type val_type, Token token)
		{
			std::string &tokstr = token.get_value();
			auto type = token.get_type();

			if (val_type == Type::INTEGER)
				value = std::stoi(tokstr);

			else if (val_type == Type::DOUBLE)
				value = std::stod(tokstr);

			else if (type == TokenType::BOOLEAN)
				value = tokstr == Token::reserved(Word::TRUE);

			else if (type == TokenType::STRING)
				value = lrstrip(tokstr);

			else
				value = 0;

		}

		LiteralExpr(ValueContainer value) : value(value)
		{
		}

		LiteralExpr() : value(std::monostate())
		{
		}

		bool is_empty()
		{
			return std::holds_alternative<std::monostate>(value);
		}

		Value evaluate(ExprEvaluator &eval) override
		{
			return Value(value);
		}

		ValueContainer raw_value()
		{
			return value;
		}

		ValueContainer &raw_ref()
		{
			return value;
		}

		void execute(ExprEvaluator &evaluator) override
		{
			Value evald = evaluate(evaluator);
			exec_literal(evaluator, evald);
		}

		static ExprPtr empty()
		{
			return std::make_shared<LiteralExpr>();
		}

		static void exec_literal(ExprEvaluator &evaluator, Value &val)
		{
			if (val.empty())
				return;

			switch (val.type) {
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
			return str << "{Literal Expression}";
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_LITERALEXPR_H_ */
