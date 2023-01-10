#include "LiteralExpr.h"

#include <interpreter/Interpreter.h>
#include <lexer/Token.h>
#include <memory>
#include <string>
#include <string_view>

namespace mtl
{

LiteralExpr::LiteralExpr(Type val_type, const Token &token)
{
	std::string &tokstr = unconst(token.get_value());
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

	else if (val_type == Type::TOKEN)
		value = token;
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

Value LiteralExpr::evaluate(Interpreter &context)
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

void LiteralExpr::execute(Interpreter &context)
{
	Value evald = evaluate(context);
	exec_literal(context, evald);
}

std::shared_ptr<LiteralExpr> LiteralExpr::empty()
{
	return std::make_shared<LiteralExpr>();
}

void LiteralExpr::exec_literal(Interpreter &context, Value &val)
{
	if (val.empty())
		return;

	switch (val.type()) {
	case Type::UNIT: {
		context.invoke(val.get<Unit>());
		break;
	}

	default:
		std::cout << val << std::endl;
		break;
	}
}

std::ostream& LiteralExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "{"
					<< "Literal Expression: `" << value << "`"
					<< "}");
}
}
