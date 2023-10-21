#include "Expression.h"

#include <memory>
#include <string>

#include <lexer/Token.h>
#include <util/util.h>
#include <util/string.h>
#include <expression/IdentifierExpr.h>
#include <expression/InvokeExpr.h>
#include <expression/LiteralExpr.h>
#include <expression/ListExpr.h>
#include <expression/PostfixExpr.h>

namespace mtl
{

const std::shared_ptr<LiteralExpr> Expression::NOOP = LiteralExpr::empty();

void Expression::execute(Interpreter &context)
{
	evaluate(context);
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
	return make_expr<PostfixExpr>(expr->line, expr, Tokens::EXCLAMATION);
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
