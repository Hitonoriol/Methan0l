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

Value LiteralExpr::raw_value()
{
	return value;
}

Value& LiteralExpr::raw_ref()
{
	return value;
}

std::shared_ptr<LiteralExpr> LiteralExpr::empty()
{
	return std::make_shared<LiteralExpr>();
}

std::ostream& LiteralExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "{"
					<< "Literal Expression: `" << value << "`"
					<< "}");
}
}
