#include "LiteralExpr.h"

#include <ExprEvaluator.h>
#include <Token.h>
#include <memory>
#include <string>
#include <string_view>

#include "translator/Translator.h"

namespace mtl
{

TRANSLATE(LiteralExpr)

LiteralExpr::LiteralExpr(Type val_type, Token token)
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

LiteralExpr::LiteralExpr(const Value &val) : value(val)
{
}

LiteralExpr::LiteralExpr() : value(NoValue())
{
}

bool LiteralExpr::is_empty()
{
	return value.empty();
}

Value LiteralExpr::evaluate(ExprEvaluator &eval)
{
	return Value(value);
}

Value LiteralExpr::raw_value()
{
	return value;
}

Value& LiteralExpr::raw_ref()
{
	return value;
}

void LiteralExpr::execute(ExprEvaluator &evaluator)
{
	Value evald = evaluate(evaluator);
	exec_literal(evaluator, evald);
}

std::shared_ptr<LiteralExpr> LiteralExpr::empty()
{
	return std::make_shared<LiteralExpr>();
}

void LiteralExpr::exec_literal(ExprEvaluator &evaluator, Value &val)
{
	if (val.empty())
		return;

	switch (val.type()) {
	case Type::UNIT: {
		evaluator.invoke(val.get<Unit>());
		break;
	}

	default:
		std::cout << val << std::endl;
		break;
	}
}

std::ostream& LiteralExpr::info(std::ostream &str)
{
	return Expression::info(str << "{Literal Expression: `" << value << "`}");
}
}
