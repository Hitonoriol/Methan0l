#include "Expression.h"

#include <memory>
#include <string>

#include "../Token.h"
#include "../util/util.h"
#include "IdentifierExpr.h"
#include "InvokeExpr.h"
#include "LiteralExpr.h"
#include "PostfixExpr.h"

namespace mtl
{

void Expression::execute(ExprEvaluator &evaluator)
{
	evaluate(evaluator);
}

void Expression::set_line(uint32_t line)
{
	this->line = line;
}

uint32_t Expression::get_line()
{
	return line;
}

std::string Expression::get_name(Expression *expr)
{
	if (instanceof<IdentifierExpr>(expr))
		return IdentifierExpr::get_name(expr);
	else if (instanceof<InvokeExpr>(expr))
		return get_name(try_cast<InvokeExpr>(expr).get_lhs());
	else
		throw std::runtime_error("Trying to access a name of a non-named expression");
}

ExprPtr Expression::return_expr(ExprPtr expr)
{
	return std::make_shared<PostfixExpr>(expr, TokenType::EXCLAMATION);
}

ExprPtr Expression::return_val(Value val)
{
	return return_expr(LiteralExpr::create(val));
}

std::ostream& Expression::info(std::ostream &str)
{
	str << " @ line ";
	if (line > 0)
		str << line;
	else
		str << "unknown";
	return str;
}

std::string Expression::info()
{
	std::stringstream ss;
	info(ss);
	return ss.str();
}

}
