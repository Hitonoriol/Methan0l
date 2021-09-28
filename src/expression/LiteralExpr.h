#ifndef EXPRESSION_LITERALEXPR_H_
#define EXPRESSION_LITERALEXPR_H_

#include "Expression.h"

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

			else value = 0;

		}

		LiteralExpr(ValueContainer value) : value(value) {};

		Value evaluate(ExprEvaluator &eval) override
		{
			return Value(value);
		}
};

} /* namespace mtl */

#endif /* EXPRESSION_LITERALEXPR_H_ */
