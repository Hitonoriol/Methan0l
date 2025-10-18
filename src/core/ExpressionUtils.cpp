#include "ExpressionUtils.h"

#include <structure/Value.h>
#include <expression/ListExpr.h>
#include <expression/LiteralExpr.h>
#include <expression/IdentifierExpr.h>
#include <expression/InvokeExpr.h>
#include <parser/expression/PostfixExpr.h>

namespace mtl
{
	std::string ExpressionUtils::get_name(Expression *expr)
	{
		if (instanceof<IdentifierExpr>(expr))
			return IdentifierExpr::get_name(expr);
		else if (instanceof<InvokeExpr>(expr))
			return get_name(try_cast<InvokeExpr>(expr).get_lhs());
		else
			throw std::runtime_error("Trying to access a name of a non-named expression");
	}

	ExprPtr ExpressionUtils::return_expr(ExprPtr expr)
	{
		return make_expr<PostfixExpr>(expr->get_line(), expr, Tokens::EXCLAMATION);
	}

	ExprPtr ExpressionUtils::return_val(Value val)
	{
		return return_expr(LiteralExpr::create(val));
	}

	void ExpressionUtils::for_one_or_multiple(ExprPtr list_or_single,
											  std::function<void(ExprPtr &)> action)
	{
		if (!if_instanceof<ListExpr>(*list_or_single, [&action](auto &list)
									 {
			for (auto &expr : list.raw_list())
				action(expr); }))
		{
			action(list_or_single);
		}
	}
}