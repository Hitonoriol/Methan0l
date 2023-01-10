#include <lexer/Token.h>
#include "Expression.h"

#include <memory>
#include <string>

#include "util/util.h"
#include "util/string.h"
#include "IdentifierExpr.h"
#include "InvokeExpr.h"
#include "LiteralExpr.h"
#include "ListExpr.h"
#include "PostfixExpr.h"

namespace mtl
{

void Expression::execute(Interpreter &evaluator)
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
	return make_expr<PostfixExpr>(expr->line, expr, TokenType::EXCLAMATION);
}

ExprPtr Expression::return_val(Value val)
{
	return return_expr(LiteralExpr::create(val));
}

void Expression::for_one_or_multiple(ExprPtr list_or_single,
		std::function<void(ExprPtr&)> action)
{
	if (!if_instanceof<ListExpr>(*list_or_single, [&action](auto &list) {
		for (auto &expr : list.raw_list())
			action(expr);
	})) {
		action(list_or_single);
	}
}

std::ostream& Expression::info(std::ostream &str)
{
	if (line == 0)
		return str;

	return str << " @ line " << line;
}

std::string Expression::info()
{
	sstream ss;
	info(ss);
	return tab(ss.str());
}

std::string Expression::info(ExprList &list)
{
	sstream ss;
	auto last = std::prev(list.end());
	for (auto it = list.begin(); it != list.end(); ++it) {
		ss << "* " << (*it)->info();
		if (it != last)
			ss << NL;
	}
	return ss.str();
}

}
