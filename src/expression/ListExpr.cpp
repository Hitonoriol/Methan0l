#include "ListExpr.h"

#include <deque>

#include "interpreter/Interpreter.h"
#include "structure/Value.h"
#include "CoreLibrary.h"


namespace mtl
{

ListExpr::ListExpr(ExprList exprs, bool as_set) : exprs(exprs), as_set(as_set)
{
}

ExprList& ListExpr::raw_list()
{
	return exprs;
}

Value ListExpr::evaluate(Interpreter &context)
{
	return as_set ?
			create_and_populate<ValSet>(context) :
			create_and_populate<List>(context);
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
