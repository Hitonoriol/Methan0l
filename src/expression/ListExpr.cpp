#include "ListExpr.h"

#include <deque>

#include <lang/util/hash.h>
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

bool ListExpr::is_set()
{
	return as_set;
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
