#include "LiteralExpr.h"

#include <memory>
#include <string>
#include <string_view>

#include <interpreter/Interpreter.h>
#include <lexer/Token.h>
#include <CoreLibrary.h>

namespace mtl
{

LiteralExpr::LiteralExpr(const Value &val) : value(val)
{}

LiteralExpr::LiteralExpr() : value(NoValue())
{}

bool LiteralExpr::is_empty()
{
	return value.empty();
}

Value LiteralExpr::evaluate(Interpreter &context)
{
	return value;
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

	if (val.type() == Type::UNIT)
		context.invoke(val.get<Unit>());
}

std::ostream& LiteralExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "{"
					<< "Literal Expression: `" << value << "`"
					<< "}");
}
}
