#include "ListExpr.h"

#include <deque>

#include <util/hash.h>
#include <util/containers.h>
#include <interpreter/Interpreter.h>
#include <structure/Value.h>
#include <CoreLibrary.h>


namespace mtl
{

ListExpr::ListExpr(ExprList exprs, bool as_set) : exprs(exprs), as_set(as_set)
{
}

ExprList& ListExpr::raw_list()
{
	return exprs;
}

template<typename T>
Value create_and_populate(Interpreter &context, ExprList& exprs)
{
	auto container = context.make<T>();
	return container.template as<T>([&](auto &ctr) {
		for (auto &expr : exprs)
			insert(ctr, expr->evaluate(context));
	});
}

Value ListExpr::evaluate(Interpreter &context)
{
	return as_set ?
			create_and_populate<ValSet>(context, exprs) :
			create_and_populate<List>(context, exprs);
}

std::ostream& ListExpr::info(std::ostream &str)
{
	return Expression::info(str
			<< "List Expression: " << BEG
					<< "Elements (" << exprs.size() << "): " << NL
					<< indent(Expression::info(exprs))
					<< END);
}

}
