#include "ListExpr.h"

#include <deque>

#include <methan0l/lang/util/hash.h>
#include <methan0l/util/containers.h>
#include <methan0l/interpreter/Interpreter.h>
#include <methan0l/structure/Value.h>
#include <methan0l/CoreLibrary.h>


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
